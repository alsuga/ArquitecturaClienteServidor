ZRep funciona con una arquitectura compuesta por un Servidor de musica (Worker), un Broker y un cliente que solicita la musica. 

*Worker:
100%  Envia la lista de las canciones.   
100%  Envia las replicas.  
100%  Recibir replicas.
100%  Envia las canciones al Broker, este luego las envia al cliente. 
 

*Broker:
100% Realiza las replicas de las canciones (Llevar contador con las canciones mas reproducidas). 
0%  Deberia de manejar la carga, conocer a cuantos clientes esta atendiendo. 

*Client:
30% Realiza peticiones de musica (Siempre y cuando la reproducion haya terminado).
0%  Realiza las funciones de reproduccion (Reproducir, pausar ...)
  
