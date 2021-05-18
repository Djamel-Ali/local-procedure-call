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
* Pour lancer plusieurs instances du programme **client** en parallèle, vous pouvez utiliser la commande intégrée d'attente (du **bash shell**) avec le symbol **&**.

voici un exemple de lancement de 2 clients en parallèle :

```bash
$>./client <shmo_name> & ./client <shmo_name>
```



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


### Voir aussi :

- [Auteurs](AUTHORS.md)
- [Dépôt git (gaufre)](https://gaufre.informatique.univ-paris-diderot.fr/alid/prog-system-av-project)
