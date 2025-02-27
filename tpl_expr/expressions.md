# Validité des expressions

## Cas classique d'un `axpy`

Prenons comme exemple une fonction retournant l'expression `axpy` :

$$
  a\cdot x + y
$$

où $x$ et $y$ sont des vecteurs et $a$ un réel. Pour construire un arbre d'expressions on peut se tourner soit vers la STL avec le `std::valarray`, soit une bibliothèque tierce telle que Eigen ou xtensor.

Dans toutes nos fonctions, $x$ et $y$ seront passés par références constantes, on s'intéresse au passage de $a$ par valeur ou référence.

Ainsi on écrit 2 fonctions, une `axpy_by_value` où la variable $a$ est passée par valeur, et une `axpy_by_reference` où $a$ est passée par référence constante :

```cpp
  template <typename container_t, typename value_t>
  auto axpy_by_value(value_t a, container_t const &x, container_t const &y) {
    return a * x + y;
  }

  template <typename container_t, typename value_t>
  auto axpy_by_reference(value_t const &a, container_t const &x,
                        container_t const &y) {
    return a * x + y;
  }
```

Puisque ces fonctions prennent une valeur ou une référence constante il est possible de les utiliser avec une variable :

```cpp
  double a = 2.0;
  axpy_by_***(a, x, y);
```

ou une valeur :

```cpp
  axpy_by_***(2.0, x, y);
```

On obtient alors les résultats suivants selon le type de conteneur :

* `std::valarray<double>` :
  ```
    [a by val]    a*x + y     	nan nan nan
    [2 by val]    2*x + y     	nan nan nan
    [a by ref]    a*x + y     	0 3 6
    [2 by ref]    2*x + y     	0 3 6
  ```

  il est donc important pour que l'expression soit bien construite de passer $a$ par référence.

* `Eigen::Vector<double, 3>` (les résultats sont les mêmes avec un `Eigen::VectorXd`) :
  ```
    [a by val]    a*x + y     	0 3 6
    [2 by val]    2*x + y     	0 3 6
    [a by ref]    a*x + y     	0 3 6
    [2 by ref]    2*x + y     	0 3 6
  ```

  on observe pas de différence, Eigen stocke sans doute les nombres flottants dans ses expressions.

* `xt::xarray<double>` :
  ```
    [a by val]    a*x + y     	0 1 2
    [2 by val]    2*x + y     	0 1 2
    [a by ref]    a*x + y     	0 3 6
    [2 by ref]    2*x + y     	0 3 6
  ```

  lorsque $a$ est passé par valeur, celui-ci semble initialisé à sa valeur par défaut, c'est-à-dire `0.`, mais l'erreur peut être compliquée à débusquer dans un code de calcul.

* `xt::xtensor_fixed<double, xt::xshape<1, 3>>` :
  ```
    [a by val]    a*x + y     	nan nan nan
    [2 by val]    2*x + y     	nan nan nan
    [a by ref]    a*x + y     	0 3 6
    [2 by ref]    2*x + y     	0 3 6
  ```
  on retrouve le même comportement que le `std::valarray`, comportement qui me semble préférable car engendrant une erreur d'arithmétique plutôt qu'un résultat faux.


## Cas d'un `abxpy`

Cette étude a été initiée pour comprendre pourquoi dans ponio je n'arrivais pas à retourner une expression (avec des `std::valarray`) sans avoir des `NaN`, mais l'expression que je calculais était légèrement différente :

$$
  a \cdot b \cdot x + y
$$

et ce produit $ab$ pose problème. La première implémentation que nous proposons, au vu des résultats précédents, est de passer $a$ et $b$ par référence, le code s'écrit comme suit :

```cpp
  template <typename container_t, typename value_t>
  auto abxpy_by_reference(value_t const &a, value_t const &b,
                          container_t const &x, container_t const &y) {
    return a * b * x + y;
  }
```

On obtient alors les résultats suivants selon le type de conteneur :

* `std::valarray<double>` :

  ```
    [a, b by ref] a*b*x + y   	nan nan nan
  ```

  les variables $a$ et $b$ sont passées par référence mais le comportement n'est pas celui attendu. En effet il semble que le calcul `a * b` soit prioritaire et ne donne au système d'expression qu'une valeur (ou référence temporaire n'existant que dans la fonction ?) du produit. On semble pouvoir décomposer le calcul dans les 3 étapes suivantes :

  ```mermaid
  graph TD;
      a["a"]
      b["b"]
      x["x"]
      y["y"]
      ab["1 : a * b"]
      abx["2 : (a * b) * x"]
      abxpy["3 : ((a * b) * x) + y"]
      a --> ab
      b --> ab
      ab -- temporary value --> abx
      x --> abx
      abx --> abxpy
      y --> abxpy
  ```

* tous les autres :

  ```
    [a, b by ref] a*b*x + y   	0 3 6
  ```

  dans les autres systèmes d'expressions l'opération `a*b` n'est pas évaluée avant d'entrer dans le système d'expression de la bibliothèque en question et on obtient le même résultat.


### Comment contourner ce problème ?

Deux solutions ont été trouvées pour résoudre ce problème avec le `std::valarray<double>`, forcer une autre décomposition de l'expression en calculant :

$$
  a \cdot ( b \cdot x) + y
$$

(effectué dans la fonction `abxpy_by_reference2`) ou alors d'éviter d'avoir deux nombres flottants à côté (ce qui revient au même) :

$$
  a \cdot x \cdot b + y
$$

(effectué dans la fonction `axbpy_by_reference`) dans ces deux cas l'expression du `std::valarray<double>` redevient juste.

### Petit fait amusant

Nous avions vu que xtensor n'avait pas de résultats corrects lors que $a$ était passé par valeur dans le cadre du `axpy`, mais dans le contexte du `abxpy` ce résultat devient juste. Je n'explique pas ce comportement.

### Tous les résultats

```
std::valarray<double>:
[a by val]    a*x + y     	nan nan nan
[2 by val]    2*x + y     	nan nan nan
[a by ref]    a*x + y     	0 3 6
[2 by ref]    2*x + y     	0 3 6
[a, b by val] a*b*x + y   	nan nan nan
[a, b by ref] a*b*x + y   	nan nan nan
[a, b by ref] a*(b*x) + y 	0 3 6
[a, b by ref] a*x*b + y   	0 3 6

Eigen::Vector<double, 3>:
[a by val]    a*x + y     	0 3 6
[2 by val]    2*x + y     	0 3 6
[a by ref]    a*x + y     	0 3 6
[2 by ref]    2*x + y     	0 3 6
[a, b by val] a*b*x + y   	0 3 6
[a, b by ref] a*b*x + y   	0 3 6
[a, b by ref] a*(b*x) + y 	0 3 6
[a, b by ref] a*x*b + y   	0 3 6

Eigen::VectorXd:
[a by val]    a*x + y     	0 3 6
[2 by val]    2*x + y     	0 3 6
[a by ref]    a*x + y     	0 3 6
[2 by ref]    2*x + y     	0 3 6
[a, b by val] a*b*x + y   	0 3 6
[a, b by ref] a*b*x + y   	0 3 6
[a, b by ref] a*(b*x) + y 	0 3 6
[a, b by ref] a*x*b + y   	0 3 6

xt::xarray<double>:
[a by val]    a*x + y     	0 1 2
[2 by val]    2*x + y     	0 1 2
[a by ref]    a*x + y     	0 3 6
[2 by ref]    2*x + y     	0 3 6
[a, b by val] a*b*x + y   	0 3 6
[a, b by ref] a*b*x + y   	0 3 6
[a, b by ref] a*(b*x) + y 	0 3 6
[a, b by ref] a*x*b + y   	0 3 6

xt::xtensor_fixed<double, xt::xshape<1, 3>>:
[a by val]    a*x + y     	nan nan nan
[2 by val]    2*x + y     	nan nan nan
[a by ref]    a*x + y     	0 3 6
[2 by ref]    2*x + y     	0 3 6
[a, b by val] a*b*x + y   	0 3 6
[a, b by ref] a*b*x + y   	0 3 6
[a, b by ref] a*(b*x) + y 	0 3 6
[a, b by ref] a*x*b + y   	0 3 6

xt::xtensor_fixed<double, xt::xshape<3, 1>>:
[a by val]    a*x + y     	nan nan nan
[2 by val]    2*x + y     	nan nan nan
[a by ref]    a*x + y     	0 3 6
[2 by ref]    2*x + y     	0 3 6
[a, b by val] a*b*x + y   	0 3 6
[a, b by ref] a*b*x + y   	0 3 6
[a, b by ref] a*(b*x) + y 	0 3 6
[a, b by ref] a*x*b + y   	0 3 6
```
