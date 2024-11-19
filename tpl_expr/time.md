# Performance des expressions

Dans [ponio](https://github.com/hpc-maths/ponio) je voulais faire une optimisation consistant à renvoyer une expression plutôt qu'un conteneur contenant l'évaluation de l'expression.

Ce mécanisme est synthétisé dans les expressions :

```cpp
  container_t fu = f(abxpy(a, b, x, y));
```

correspondant au calcul $k_i = f(u^n + \Delta t \sum_j a_{ij}k_j)$, l'expression $\sum_j a_{ij}k_j$ correspondant à un produit scalaire, avec un $a$ variable et donc peut se résumer à un `axpy`, ici on rajoute un scalaire $b$ correspondant au $\Delta t$. La fonction $f$ est construite ici pour prendre une *r-value*, et renvoyer une expression qui sera nulle quoi qu'il arrive (`0.5*u - u + 0.5*u`).

On souhaite tester la différence dans la fonction `abxpy` qui calcule : $a\cdot b \cdot x + y$, entre renvoyer une expression et un conteneur. Deux fonctions sont construites pour tester cela :

```cpp
template <typename container_t, typename value_t>
auto abxpy_auto(value_t const &a, value_t const &b, container_t const &x,
                container_t const &y) {
  return a * (b * x) + y;
}

template <typename container_t, typename value_t>
container_t abxpy_container(value_t const &a, value_t const &b,
                            container_t const &x, container_t const &y) {
  return a * (b * x) + y;
}
```

On lance 50000 fois ce test, avec des vecteurs de taille 1000, et on obtient, pour différents types de conteneurs :

```
std::valarray<double>:
abxpy_container 2.17358 50000
abxpy_auto 3.74757 50000

xt::xarray<double>:
abxpy_container 0.183311 50000
abxpy_auto 0.223934 50000

Eigen::VectorXd:
abxpy_container 2.71495 50000
abxpy_auto 4.42228 50000
```
