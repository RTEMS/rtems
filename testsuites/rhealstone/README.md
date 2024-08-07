Rhealstone
==========
Rhealstone Benchmark For RTEMS

This is an implementation of the Rhealstone benchmarks for RTEMS.

The Rhealstone metric is a set of benchmarks that aim to provide an
independently verifiable objective measure of real-time performance in
different systems. The Rhealstone metric is composed of six independent
benchmarks, each of which measure an activity that is critical in real
time systems:

* Task switch time
* Task preemption time
* Interrupt latency time
* Semaphore shuffle time
* Deadlock breaking time
* Intertask message latency

An implementation of the Rhealstone metric allows for RTEMS performance to be
compared with other real-time systems.

The original proposal outlining the Rhealstone benchmark can be found
http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/DDJ/1989/8902/8902a/8902a.htm[here].

Some other implementations can be seen in
http://timsengineeringblog.weebly.com/masters-thesis.html[FreeRTOS] and
http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/DDJ/1990/9004/9004d/9004d.htm[iRMX].
