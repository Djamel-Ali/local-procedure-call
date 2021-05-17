#ifndef __FUN_HELLO_H__
#define __FUN_HELLO_H__

/**
 * @brief Prend en paramètre un *lpc_string contenant un nom <name> (généralement C'est le pid du client),
 * et construit la chaine "Bonjour <name>, je suis Serveur <pid_of_current_server>".
 *
 * @return int (0 in success, else -1)
 */
int hello(void *);

#endif  // __FUN_HELLO_H__