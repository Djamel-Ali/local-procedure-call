#ifndef __LPC_SERVER_H__
#define __LPC_SERVER_H__

#include <sys/types.h>

#include "memory.h"

typedef struct {
  /**
   * @brief nom de la fonction
   *
   */
  char fun_name[NAMELEN];

  /**
   * @brief
   * L'adresse de la fonction à appeler.
   * La fonction retourne 0 quand l'appel réussit, -1 sinon.
   *
   */
  int (*fun)(void *);
} lpc_function;

/**
 * @brief Crée un objet shared memory object de nom [name] et de taille
 * [capacite] * [taille_de_page] octets. Si l'objet existe alors sa taille est
 * modifiée. lpc_create retourne une projection mémoire de type [memory*] de
 * l'objet crée après initialisation de la partie [header], ou NULL en cas
 * d'échec.
 *
 * @param nom
 * @param capacite
 * @return memory*
 */
memory *lpc_create(const char *shmo_name, size_t capacity);

/**
 * @brief Retourne le pointeur de fonction correspondant à la fonction [fun_name]
 * si elle existe, NULL sinon
 * 
 * @param fun_name 
 * @return void* 
 */
void *lpc_get_fun(const char *fun_name);

#endif  // __LPC_SERVER_H__