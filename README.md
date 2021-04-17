# Local Procedure Call - LPC

Inspiré de RPC (Remote Procedure Call), LPC permet à un processus client d'appeler une fonction
implémentée dans un autre processus, le processus serveur.

### Utilisation

**Comiplation**

Un makefile est fourni à la racine du projet. Compilez avec la commande:
```bash
$> make
```

**Exécution**
Exécutez d'abord le serveur avec la commande:
```bash
$>./server
```
Puis exécuter les clients
```bash
$>./test
```

**Autres règles du makefile**
Supprimer les fichiers objets:
```bash
$>make clean
```
Supprimer les dépendances:
```bash
$>make cleandep
```
Supprimer tous les fichiers générés:
```bash
$>make cleanall
```


### Voir 

- [Rapport](docs/Rapport.pdf)
- [Auteurs](AUTHORS.md)
