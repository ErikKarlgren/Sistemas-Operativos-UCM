# listar procesos del sistema: ps -aux
# solo los de usuarios cuyo nombre empieza por r

# con 'ps -aux | grep r' nos quedaríamos con todas las líneas con r, no las que empiezan por r
# hay que usar expresiones regulares (ver diapositiva 24 del tema de shell)

# así que a grep le tenemos que pasar como argumento '^r'

ps -aux | grep "^r"

# si quisiéramos filtrar las líneas que tienen palabras que empiezan por r PERO que tienen un espacio antes...
ps -aux | grep " r"

# otro programa interesante para filtrar datos es 'cut', que lo que hace es cortar cosas
# podemos quedarnos solo con las columnas que nos interesan por ejemplo, en vez de todas
