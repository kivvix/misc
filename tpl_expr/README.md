# À propos d'expressions templates en C++

Pour exécuter le code associé :

```sh
  conda env create -f environment.yml
  conda activate misc-tpl_expr
```

puis on compile et on exécute le code, simplement avec :

```sh
  make
```

Une partie s'intéresse également au temps de restitution de ces expressions, pour l'exécuter il suffit de faire :

```sh
  make time.png
```

Le travail sur ces expressions templates est synthétisé dans plusieurs fichiers Markdown :

* [Validité des expressions](expressions.md) où des tests sont faits sur la validité d'une expression renvoyée par une fonction (`axpy`).
* [Performance des expressions](time.md) où des comparaisons sont faites entre renvoyer une expression ou son évaluation.
