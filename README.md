# Local Procedure Call - LPC

Inspiré de RPC (Remote Procedure Call), LPC permet à un processus client d'appeler une fonction
implémentée dans un autre processus, le processus serveur.

## Utilisation

### Comiplation

Un makefile est fourni à la racine du projet. Compilez avec la commande:
```bash
$> make
```

### Exécution
_(**Remarque :** il faut se placer d'abord dans le dossier racine de ce projet)._

* D'abord, exécutez le serveur avec la commande :
```bash
$>./server <shmo_name>
```
* Puis exécuter les clients, avec la commande ci-dessous pour chaque client :

_(**Attention :** il faut bien sûr donner le même nom **<shmo_name>** que celui utilisé lors de l'exécution du serveur)_
```bash
$>./client <shmo_name>
```
Sachant que les programmes **"client"** sont les fichiers exécutables dans le dossier _tests/_.


### Autres règles du Makefile

* Suppression des fichiers objets (*.o) :
```bash
$>make clean
```
* Suppression des fichiers de dépendances (*.d) :
```bash
$>make cleandep
```
* Suppression de tous les fichiers générés (exécutables, *.o et *.d) :
```bash
$>make cleanall
```

### Détails d'implémentation
* Nous avons implémenté un serveur distribué (et donc qui peut prendre en charge plusieurs clients).
* Quand un client demande une fonction qui n'existe pas dans le tableau de fonctions gérées par le serveur,
ce dernier retourne un code d'erreur **ENOENT**.
* On s'est assuré d'avoir bien libéré la mémoire allouée dynamiquement aussi bien du côté serveur que du côté client.
* Nous avons mis en place des fonctions de test (dans _tests/_) qui illustrent les cas de succès et d'échec pour chacune.
Ces fonctions utilisent tous les types proposés dans ce projet.

### License & copyright
© [Djamel ALI](https://github.com/DjamelALI) & Mamadou Alpha BARRY, Université de Paris

Licensed under the [MIT License](LICENSE).

### Voir aussi :

- [Auteurs](AUTHORS.md)
- [Dépôt git (gaufre)](https://gaufre.informatique.univ-paris-diderot.fr/alid/prog-system-av-project)

