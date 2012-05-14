/**
 * @file rtems/rtems/mainpage.h
 *
 * This file exists to provide a top level description of RTEMS for Doxygen.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/**
 * @mainpage
 *
 * The RTEMS real-time operating systems is a layered system with each of the
 * public APIs implemented in terms of a common foundation layer called the
 * SuperCore.  This is the Doxygen generated documentation for the RTEMS CPU
 * Kit including the Classic API, POSIX API and SuperCore.
 */

/**
 * @page RTEMSPreface RTEMS History and Introduction
 *
 * In recent years, the cost required to develop a software product has
 * increased significantly while the target hardware costs have decreased. Now
 * a larger portion of money is expended in developing, using, and maintaining
 * software. The trend in computing costs is the complete dominance of software
 * over hardware costs. Because of this, it is necessary that formal
 * disciplines be established to increase the probability that software is
 * characterized by a high degree of correctness, maintainability, and
 * portability. In addition, these disciplines must promote practices that aid
 * in the consistent and orderly development of a software system within
 * schedule and budgetary constraints. To be effective, these disciplines must
 * adopt standards which channel individual software efforts toward a common
 * goal.
 *
 * The push for standards in the software development field has been met with
 * various degrees of success. The Microprocessor Operating Systems Interfaces
 * (MOSI) effort has experienced only limited success. As popular as the UNIX
 * operating system has grown, the attempt to develop a standard interface
 * definition to allow portable application development has only recently begun
 * to produce the results needed in this area. Unfortunately, very little
 * effort has been expended to provide standards addressing the needs of the
 * real-time community. Several organizations have addressed this need during
 * recent years.
 *
 * The Real Time Executive Interface Definition (RTEID) was developed by
 * Motorola with technical input from Software Components Group. RTEID was
 * adopted by the VMEbus International Trade Association (VITA) as a baseline
 * draft for their proposed standard multiprocessor, real-time executive
 * interface, Open Real-Time Kernel Interface Definition (ORKID). These two
 * groups are currently working together with the IEEE P1003.4 committee to
 * insure that the functionality of their proposed standards is adopted as the
 * real-time extensions to POSIX.
 *
 * This emerging standard defines an interface for the development of real-time
 * software to ease the writing of real-time application programs that are
 * directly portable across multiple real-time executive implementations. This
 * interface includes both the source code interfaces and run-time behavior as
 * seen by a real-time application. It does not include the details of how a
 * kernel implements these functions. The standard's goal is to serve as a
 * complete definition of external interfaces so that application code that
 * conforms to these interfaces will execute properly in all real-time
 * executive environments. With the use of a standards compliant executive,
 * routines that acquire memory blocks, create and manage message queues,
 * establish and use semaphores, and send and receive signals need not be
 * redeveloped for a different real-time environment as long as the new
 * environment is compliant with the standard. Software developers need only
 * concentrate on the hardware dependencies of the real-time system.
 * Furthermore, most hardware dependencies for real-time applications can be
 * localized to the device drivers.
 *
 * A compliant executive provides simple and flexible real-time
 * multiprocessing. It easily lends itself to both tightly-coupled and
 * loosely-coupled configurations (depending on the system hardware
 * configuration). Objects such as tasks, queues, events, signals, semaphores,
 * and memory blocks can be designated as global objects and accessed by any
 * task regardless of which processor the object and the accessing task reside.
 *
 * The acceptance of a standard for real-time executives will produce the same
 * advantages enjoyed from the push for UNIX standardization by AT&T's System V
 * Interface Definition and IEEE's POSIX efforts. A compliant multiprocessing
 * executive will allow close coupling between UNIX systems and real-time
 * executives to provide the many benefits of the UNIX development environment
 * to be applied to real-time software development. Together they provide the
 * necessary laboratory environment to implement real-time, distributed,
 * embedded systems using a wide variety of computer architectures.
 *
 * A study was completed in 1988, within the Research, Development, and
 * Engineering Center, U.S. Army Missile Command, which compared the various
 * aspects of the Ada programming language as they related to the application
 * of Ada code in distributed and/or multiple processing systems. Several
 * critical conclusions were derived from the study. These conclusions have a
 * major impact on the way the Army develops application software for embedded
 * applications. These impacts apply to both in-house software development and
 * contractor developed software.
 *
 * A conclusion of the analysis, which has been previously recognized by other
 * agencies attempting to utilize Ada in a distributed or multiprocessing
 * environment, is that the Ada programming language does not adequately
 * support multiprocessing. Ada does provide a mechanism for multi-tasking,
 * however, this capability exists only for a single processor system. The
 * language also does not have inherent capabilities to access global named
 * variables, flags or program code. These critical features are essential in
 * order for data to be shared between processors. However, these drawbacks do
 * have workarounds which are sometimes awkward and defeat the intent of
 * software maintainability and portability goals.
 *
 * Another conclusion drawn from the analysis, was that the run time executives
 * being delivered with the Ada compilers were too slow and inefficient to be
 * used in modern missile systems. A run time executive is the core part of the
 * run time system code, or operating system code, that controls task
 * scheduling, input/output management and memory management. Traditionally,
 * whenever efficient executive (also known as kernel) code was required by the
 * application, the user developed in-house software. This software was usually
 * written in assembly language for optimization.
 *
 * Because of this shortcoming in the Ada programming language, software
 * developers in research and development and contractors for project managed
 * systems, are mandated by technology to purchase and utilize off-the-shelf
 * third party kernel code. The contractor, and eventually the Government, must
 * pay a licensing fee for every copy of the kernel code used in an embedded
 * system.
 *
 * The main drawback to this development environment is that the Government
 * does not own, nor has the right to modify code contained within the kernel.
 * V&V techniques in this situation are more difficult than if the complete
 * source code were available. Responsibility for system failures due to faulty
 * software is yet another area to be resolved under this environment.
 *
 * The Guidance and Control Directorate began a software development effort to
 * address these problems. A project to develop an experimental run time kernel
 * was begun that will eliminate the major drawbacks of the Ada programming
 * language mentioned above. The Real Time Executive for Multiprocessor Systems
 * (RTEMS) provides full capabilities for management of tasks, interrupts,
 * time, and multiple processors in addition to those features typical of
 * generic operating systems. The code is Government owned, so no licensing
 * fees are necessary. RTEMS has been implemented in both the Ada and C
 * programming languages. It has been ported to the following processor
 * families:
 *
 * - Altera NIOS II
 * - Analog Devices Blackfin
 * - ARM
 * - Freescale (formerly Motorola) MC68xxx
 * - Freescale (formerly Motorola) MC683xx
 * - Freescale (formerly Motorola) ColdFire
 * - Intel i386 and above
 * - Lattice Semiconductor LM32
 * - MIPS
 * - PowerPC
 * - Renesas (formerly Hitachi) SuperH
 * - Renesas (formerly Hitachi) H8/300
 * - SPARC
 * - Texas Instruments C3x/C4x
 * - UNIX 
 *
 * Support for other processor families, including RISC, CISC, and DSP, is
 * planned. Since almost all of RTEMS is written in a high level language,
 * ports to additional processor families require minimal effort.
 *
 * RTEMS multiprocessor support is capable of handling either homogeneous or
 * heterogeneous systems. The kernel automatically compensates for
 * architectural differences (byte swapping, etc.) between processors. This
 * allows a much easier transition from one processor family to another without
 * a major system redesign.
 *
 * Since the proposed standards are still in draft form, RTEMS cannot and does
 * not claim compliance. However, the status of the standard is being carefully
 * monitored to guarantee that RTEMS provides the functionality specified in
 * the standard. Once approved, RTEMS will be made compliant.
 */

/**
 * @page RTEMSOverview RTEMS Overview
 *
 * @section RTEMSOverviewSecIntroduction Introduction
 *
 * RTEMS, Real-Time Executive for Multiprocessor Systems, is a real-time
 * executive (kernel) which provides a high performance environment for
 * embedded military applications including the following features:
 *
 * - multitasking capabilities
 * - homogeneous and heterogeneous multiprocessor systems
 * - event-driven, priority-based, preemptive scheduling
 * - optional rate monotonic scheduling
 * - intertask communication and synchronization
 * - priority inheritance
 * - responsive interrupt management
 * - dynamic memory allocation
 * - high level of user configurability
 *
 * This manual describes the usage of RTEMS for applications written in the C
 * programming language. Those implementation details that are processor
 * dependent are provided in the Applications Supplement documents. A
 * supplement document which addresses specific architectural issues that
 * affect RTEMS is provided for each processor type that is supported.
 *
 * @section RTEMSOverviewSecRealtimeApplicationSystems Real-time Application Systems
 *
 * Real-time application systems are a special class of computer applications.
 * They have a complex set of characteristics that distinguish them from other
 * software problems. Generally, they must adhere to more rigorous
 * requirements. The correctness of the system depends not only on the results
 * of computations, but also on the time at which the results are produced. The
 * most important and complex characteristic of real-time application systems
 * is that they must receive and respond to a set of external stimuli within
 * rigid and critical time constraints referred to as deadlines. Systems can be
 * buried by an avalanche of interdependent, asynchronous or cyclical event
 * streams.
 *
 * Deadlines can be further characterized as either hard or soft based upon the
 * value of the results when produced after the deadline has passed. A deadline
 * is hard if the results have no value or if their use will result in a
 * catastrophic event. In contrast, results which are produced after a soft
 * deadline may have some value.
 *
 * Another distinguishing requirement of real-time application systems is the
 * ability to coordinate or manage a large number of concurrent activities.
 * Since software is a synchronous entity, this presents special problems. One
 * instruction follows another in a repeating synchronous cycle. Even though
 * mechanisms have been developed to allow for the processing of external
 * asynchronous events, the software design efforts required to process and
 * manage these events and tasks are growing more complicated.
 *
 * The design process is complicated further by spreading this activity over a
 * set of processors instead of a single processor. The challenges associated
 * with designing and building real-time application systems become very
 * complex when multiple processors are involved. New requirements such as
 * interprocessor communication channels and global resources that must be
 * shared between competing processors are introduced. The ramifications of
 * multiple processors complicate each and every characteristic of a real-time
 * system.
 *
 * @section RTEMSOverviewSecRealtimeExecutive Real-time Executive
 *
 * Fortunately, real-time operating systems or real-time executives serve as a
 * cornerstone on which to build the application system. A real-time
 * multitasking executive allows an application to be cast into a set of
 * logical, autonomous processes or tasks which become quite manageable. Each
 * task is internally synchronous, but different tasks execute independently,
 * resulting in an asynchronous processing stream. Tasks can be dynamically
 * paused for many reasons resulting in a different task being allowed to
 * execute for a period of time. The executive also provides an interface to
 * other system components such as interrupt handlers and device drivers.
 * System components may request the executive to allocate and coordinate
 * resources, and to wait for and trigger synchronizing conditions. The
 * executive system calls effectively extend the CPU instruction set to support
 * efficient multitasking. By causing tasks to travel through well-defined
 * state transitions, system calls permit an application to demand-switch
 * between tasks in response to real-time events.
 *
 * By proper grouping of responses to stimuli into separate tasks, a system can
 * now asynchronously switch between independent streams of execution, directly
 * responding to external stimuli as they occur. This allows the system design
 * to meet critical performance specifications which are typically measured by
 * guaranteed response time and transaction throughput. The multiprocessor
 * extensions of RTEMS provide the features necessary to manage the extra
 * requirements introduced by a system distributed across several processors.
 * It removes the physical barriers of processor boundaries from the world of
 * the system designer, enabling more critical aspects of the system to receive
 * the required attention. Such a system, based on an efficient real-time,
 * multiprocessor executive, is a more realistic model of the outside world or
 * environment for which it is designed. As a result, the system will always be
 * more logical, efficient, and reliable.
 *
 * By using the directives provided by RTEMS, the real-time applications
 * developer is freed from the problem of controlling and synchronizing
 * multiple tasks and processors. In addition, one need not develop, test,
 * debug, and document routines to manage memory, pass messages, or provide
 * mutual exclusion. The developer is then able to concentrate solely on the
 * application. By using standard software components, the time and cost
 * required to develop sophisticated real-time applications is significantly
 * reduced.
 *
 * @section RTEMSOverviewSecApplicationArchitecture RTEMS Application Architecture
 *
 * One important design goal of RTEMS was to provide a bridge between two
 * critical layers of typical real-time systems. As shown in the following
 * figure, RTEMS serves as a buffer between the project dependent application
 * code and the target hardware. Most hardware dependencies for real-time
 * applications can be localized to the low level device drivers.
 *
 * @todo Image RTEMS Application Architecture
 *
 * The RTEMS I/O interface manager provides an efficient tool for incorporating
 * these hardware dependencies into the system while simultaneously providing a
 * general mechanism to the application code that accesses them. A well
 * designed real-time system can benefit from this architecture by building a
 * rich library of standard application components which can be used repeatedly
 * in other real-time projects.
 *
 * @section RTEMSOverviewSecInternalArchitecture RTEMS Internal Architecture
 *
 * RTEMS can be viewed as a set of layered components that work in harmony to
 * provide a set of services to a real-time application system. The executive
 * interface presented to the application is formed by grouping directives into
 * logical sets called resource managers. Functions utilized by multiple
 * managers such as scheduling, dispatching, and object management are provided
 * in the executive core. The executive core depends on a small set of CPU
 * dependent routines. Together these components provide a powerful run time
 * environment that promotes the development of efficient real-time application
 * systems. The following figure illustrates this organization:
 *
 * @todo Image RTEMS Architecture
 *
 * Subsequent chapters present a detailed description of the capabilities
 * provided by each of the following RTEMS managers:
 *
 * - initialization
 * - task
 * - interrupt
 * - clock
 * - timer
 * - semaphore
 * - message
 * - event
 * - signal
 * - partition
 * - region
 * - dual ported memory
 * - I/O
 * - fatal error
 * - rate monotonic
 * - user extensions
 * - multiprocessing
 *
 * @section RTEMSOverviewSecUserCustomization User Customization and Extensibility
 *
 * As 32-bit microprocessors have decreased in cost, they have become
 * increasingly common in a variety of embedded systems. A wide range of custom
 * and general-purpose processor boards are based on various 32-bit
 * processors. RTEMS was designed to make no assumptions concerning the
 * characteristics of individual microprocessor families or of specific support
 * hardware. In addition, RTEMS allows the system developer a high degree of
 * freedom in customizing and extending its features.
 *
 * RTEMS assumes the existence of a supported microprocessor and sufficient
 * memory for both RTEMS and the real-time application. Board dependent
 * components such as clocks, interrupt controllers, or I/O devices can be
 * easily integrated with RTEMS. The customization and extensibility features
 * allow RTEMS to efficiently support as many environments as possible.
 *
 * @section RTEMSOverviewSecPortability Portability
 *
 * The issue of portability was the major factor in the creation of RTEMS.
 * Since RTEMS is designed to isolate the hardware dependencies in the specific
 * board support packages, the real-time application should be easily ported to
 * any other processor. The use of RTEMS allows the development of real-time
 * applications which can be completely independent of a particular
 * microprocessor architecture.
 *
 * @section RTEMSOverviewSecMemoryRequirements Memory Requirements
 *
 * Since memory is a critical resource in many real-time embedded systems,
 * RTEMS was specifically designed to automatically leave out all services that
 * are not required from the run-time environment. Features such as networking,
 * various fileystems, and many other features are completely optional. This
 * allows the application designer the flexibility to tailor RTEMS to most
 * efficiently meet system requirements while still satisfying even the most
 * stringent memory constraints. As a result, the size of the RTEMS executive
 * is application dependent.
 *
 * RTEMS requires RAM to manage each instance of an RTEMS object that is
 * created. Thus the more RTEMS objects an application needs, the more memory
 * that must be reserved. See Configuring a System Determining Memory
 * Requirements for more details.
 *
 * @todo Link to Configuring a SystemDetermining Memory Requirements
 *
 * RTEMS utilizes memory for both code and data space. Although RTEMS' data
 * space must be in RAM, its code space can be located in either ROM or RAM.
 *
 * @section RTEMSOverviewSecAudience Audience
 *
 * This manual was written for experienced real-time software developers.
 * Although some background is provided, it is assumed that the reader is
 * familiar with the concepts of task management as well as intertask
 * communication and synchronization. Since directives, user related data
 * structures, and examples are presented in C, a basic understanding of the C
 * programming language is required to fully understand the material presented.
 * However, because of the similarity of the Ada and C RTEMS implementations,
 * users will find that the use and behavior of the two implementations is very
 * similar. A working knowledge of the target processor is helpful in
 * understanding some of RTEMS' features. A thorough understanding of the
 * executive cannot be obtained without studying the entire manual because many
 * of RTEMS' concepts and features are interrelated. Experienced RTEMS users
 * will find that the manual organization facilitates its use as a reference
 * document.
 *
 */

/**
 * @addtogroup ClassicRTEMS
 *
 * The facilities provided by RTEMS are built upon a foundation of very
 * powerful concepts. These concepts must be understood before the application
 * developer can efficiently utilize RTEMS. The purpose of this chapter is to
 * familiarize one with these concepts.
 *
 * @section ClassicRTEMSSecObjects Objects
 *
 * RTEMS provides directives which can be used to dynamically create, delete,
 * and manipulate a set of predefined object types. These types include tasks,
 * message queues, semaphores, memory regions, memory partitions, timers,
 * ports, and rate monotonic periods. The object-oriented nature of RTEMS
 * encourages the creation of modular applications built upon re-usable
 * "building block" routines.
 *
 * All objects are created on the local node as required by the application and
 * have an RTEMS assigned ID. All objects have a user-assigned name. Although a
 * relationship exists between an object's name and its RTEMS assigned ID, the
 * name and ID are not identical. Object names are completely arbitrary and
 * selected by the user as a meaningful "tag" which may commonly reflect the
 * object's use in the application. Conversely, object IDs are designed to
 * facilitate efficient object manipulation by the executive.
 * 
 * @subsection ClassicRTEMSSubSecObjectNames Object Names
 *
 * An object name is an unsigned 32-bit entity associated with the
 * object by the user. The data type @ref rtems_name is used to store object names.
 *
 * Although not required by RTEMS, object names are often composed of four
 * ASCII characters which help identify that object. For example, a task which
 * causes a light to blink might be called "LITE". The rtems_build_name()
 * routine is provided to build an object name from four ASCII characters. The
 * following example illustrates this:
 * 
 * @code
 * rtems_name my_name = rtems_build_name('L', 'I', 'T', 'E');
 * @endcode
 *
 * However, it is not required that the application use ASCII characters to
 * build object names. For example, if an application requires one-hundred
 * tasks, it would be difficult to assign meaningful ASCII names to each task.
 * A more convenient approach would be to name them the binary values one
 * through one-hundred, respectively.
 *
 * RTEMS provides a helper routine, rtems_object_get_name(), which can be used to
 * obtain the name of any RTEMS object using just its ID. This routine attempts
 * to convert the name into a printable string.
 * 
 * @subsection ClassicRTEMSSubSecObjectIdentifiers Object Identifiers
 *
 * An object ID is a unique unsigned integer value which uniquely identifies an
 * object instance. Object IDs are passed as arguments to many directives in
 * RTEMS and RTEMS translates the ID to an internal object pointer. The
 * efficient manipulation of object IDs is critical to the performance of RTEMS
 * services. Because of this, there are two object ID formats defined. Each
 * target architecture specifies which format it will use. There is a 32-bit
 * format which is used for most of the supported architectures and supports
 * multiprocessor configurations. There is also a simpler 16-bit format which
 * is appropriate for smaller target architectures and does not support
 * multiprocessor configurations.
 *
 * @subsubsection ClassicRTEMSSubSec32BitObjectIdentifierFormat 32-Bit Object Identifier Format
 *
 * The 32-bit format for an object ID is composed of four parts: API,
 * object class, node, and index. The data type @ref rtems_id is used to store
 * object IDs.
 *
 * <table>
 *   <tr>
 *     <th>Bits</th>
 *     <td>31</td><td>30</td><td>29</td><td>28</td><td>27</td><td>26</td><td>25</td><td>24</td>
 *     <td>23</td><td>22</td><td>21</td><td>20</td><td>19</td><td>18</td><td>17</td><td>16</td>
 *     <td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>09</td><td>08</td>
 *     <td>07</td><td>06</td><td>05</td><td>04</td><td>03</td><td>02</td><td>01</td><td>00</td>
 *   </tr>
 *   <tr>
 *     <th>Contents</th>
 *     <td colspan=5>Class</td><td colspan=3>API</td><td colspan=8>Node</td><td colspan=16>Object Index</td>
 *   </tr>
 * </table>
 *
 * The most significant five bits are the object class. The next three bits
 * indicate the API to which the object class belongs. The next eight bits
 * (16 .. 23) are the number of the node on which this object was created. The
 * node number is always one (1) in a single processor system. The least
 * significant 16-bits form an identifier within a particular object type.
 * This identifier, called the object index, ranges in value from one to the
 * maximum number of objects configured for this object type.
 *
 * @subsubsection ClassicRTEMSSubSec16BitObjectIdentifierFormat 16-Bit Object Identifier Format
 *
 * The 16-bit format for an object ID is composed of three parts: API, object
 * class, and index. The data type @ref rtems_id is used to store object IDs.
 *
 * <table>
 *   <tr>
 *     <th>Bits</th>
 *     <td>15</td><td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>09</td><td>08</td>
 *     <td>07</td><td>06</td><td>05</td><td>04</td><td>03</td><td>02</td><td>01</td><td>00</td>
 *   </tr>
 *   <tr>
 *     <th>Contents</th>
 *     <td colspan=5>Class</td><td colspan=3>API</td><td colspan=8>Object Index</td>
 *   </tr>
 * </table>
 *
 * The 16-bit format is designed to be as similar as possible to the 32-bit
 * format. The differences are limited to the eliminatation of the node field
 * and reduction of the index field from 16-bits to 8-bits.  Thus the 16-bit
 * format only supports up to 255 object instances per API/Class combination
 * and single processor systems. As this format is typically utilized by 16-bit
 * processors with limited address space, this is more than enough object
 * instances.
 *
 * @subsection ClassicRTEMSSubSecObjectIdentiferDescription Object Identifer Description
 *
 * The components of an object ID make it possible to quickly locate any object
 * in even the most complicated multiprocessor system. Object ID's are
 * associated with an object by RTEMS when the object is created and the
 * corresponding ID is returned by the appropriate object create directive. The
 * object ID is required as input to all directives involving objects, except
 * those which create an object or obtain the ID of an object.
 *
 * The object identification directives can be used to dynamically obtain a
 * particular object's ID given its name. This mapping is accomplished by
 * searching the name table associated with this object type. If the name is
 * non-unique, then the ID associated with the first occurrence of the name
 * will be returned to the application. Since object IDs are returned when the
 * object is created, the object identification directives are not necessary in
 * a properly designed single processor application.
 *
 * In addition, services are provided to portably examine the subcomponents of
 * an RTEMS ID. These services are described in detail later in this manual but
 * are prototyped as follows:
 *
 * - rtems_object_id_get_api()
 * - rtems_object_id_get_class()
 * - rtems_object_id_get_node()
 * - rtems_object_id_get_index()
 *
 * An object control block is a data structure defined by RTEMS which contains
 * the information necessary to manage a particular object type. For efficiency
 * reasons, the format of each object type's control block is different.
 * However, many of the fields are similar in function. The number of each type
 * of control block is application dependent and determined by the values
 * specified in the user's Configuration Table. An object control block is
 * allocated at object create time and freed when the object is deleted. With
 * the exception of user extension routines, object control blocks are not
 * directly manipulated by user applications.
 *
 * @section ClassicRTEMSSecComSync Communication and Synchronization
 *
 * In real-time multitasking applications, the ability for cooperating
 * execution threads to communicate and synchronize with each other is
 * imperative. A real-time executive should provide an application with the
 * following capabilities
 *
 * - data transfer between cooperating tasks,
 * - data transfer between tasks and ISRs,
 * - synchronization of cooperating tasks, and
 * - synchronization of tasks and ISRs.
 *
 * Most RTEMS managers can be used to provide some form of communication and/or
 * synchronization. However, managers dedicated specifically to communication
 * and synchronization provide well established mechanisms which directly map
 * to the application's varying needs. This level of flexibility allows the
 * application designer to match the features of a particular manager with the
 * complexity of communication and synchronization required. The following
 * managers were specifically designed for communication and synchronization:
 *
 * - @ref ClassicSem
 * - @ref ClassicMessageQueue
 * - @ref ClassicEvent
 * - @ref ClassicSignal
 *
 * The semaphore manager supports mutual exclusion involving the
 * synchronization of access to one or more shared user resources. Binary
 * semaphores may utilize the optional priority inheritance algorithm to avoid
 * the problem of priority inversion. The message manager supports both
 * communication and synchronization, while the event manager primarily
 * provides a high performance synchronization mechanism. The signal manager
 * supports only asynchronous communication and is typically used for exception
 * handling.
 *
 * @section ClassicRTEMSSecTime Time
 *
 * The development of responsive real-time applications requires an
 * understanding of how RTEMS maintains and supports time-related operations.
 * The basic unit of time in RTEMS is known as a tick. The frequency of clock
 * ticks is completely application dependent and determines the granularity and
 * accuracy of all interval and calendar time operations.
 *
 * By tracking time in units of ticks, RTEMS is capable of supporting interval
 * timing functions such as task delays, timeouts, timeslicing, the delayed
 * execution of timer service routines, and the rate monotonic scheduling of
 * tasks. An interval is defined as a number of ticks relative to the current
 * time. For example, when a task delays for an interval of ten ticks, it is
 * implied that the task will not execute until ten clock ticks have occurred.
 * All intervals are specified using data type @ref rtems_interval.
 *
 * A characteristic of interval timing is that the actual interval period may
 * be a fraction of a tick less than the interval requested. This occurs
 * because the time at which the delay timer is set up occurs at some time
 * between two clock ticks. Therefore, the first countdown tick occurs in less
 * than the complete time interval for a tick. This can be a problem if the
 * clock granularity is large.
 *
 * The rate monotonic scheduling algorithm is a hard real-time scheduling
 * methodology. This methodology provides rules which allows one to guarantee
 * that a set of independent periodic tasks will always meet their deadlines --
 * even under transient overload conditions. The rate monotonic manager
 * provides directives built upon the Clock Manager's interval timer support
 * routines.
 *
 * Interval timing is not sufficient for the many applications which require
 * that time be kept in wall time or true calendar form. Consequently, RTEMS
 * maintains the current date and time. This allows selected time operations to
 * be scheduled at an actual calendar date and time. For example, a task could
 * request to delay until midnight on New Year's Eve before lowering the ball
 * at Times Square. The data type @ref rtems_time_of_day is used to specify
 * calendar time in RTEMS services. See Clock Manager Time and Date Data
 * Structures.
 *
 * @todo Link to Clock Manager Time and Date Data Structures
 *
 * Obviously, the directives which use intervals or wall time cannot operate
 * without some external mechanism which provides a periodic clock tick. This
 * clock tick is typically provided by a real time clock or counter/timer
 * device.
 *
 * @section ClassicRTEMSSecMemoryManagement Memory Management
 *
 * RTEMS memory management facilities can be grouped into two classes: dynamic
 * memory allocation and address translation. Dynamic memory allocation is
 * required by applications whose memory requirements vary through the
 * application's course of execution. Address translation is needed by
 * applications which share memory with another CPU or an intelligent
 * Input/Output processor. The following RTEMS managers provide facilities to
 * manage memory:
 *
 * - @ref ClassicRegion
 * - @ref ClassicPart
 * - @ref ClassicDPMEM 
 *
 * RTEMS memory management features allow an application to create simple
 * memory pools of fixed size buffers and/or more complex memory pools of
 * variable size segments. The partition manager provides directives to manage
 * and maintain pools of fixed size entities such as resource control blocks.
 * Alternatively, the region manager provides a more general purpose memory
 * allocation scheme that supports variable size blocks of memory which are
 * dynamically obtained and freed by the application. The dual-ported memory
 * manager provides executive support for address translation between internal
 * and external dual-ported RAM address space.
 */

/**
 * @addtogroup ClassicTasks
 *
 * @section ClassicTasksSecTaskDefinition Task Definition
 *
 * Many definitions of a task have been proposed in computer literature.
 * Unfortunately, none of these definitions encompasses all facets of the
 * concept in a manner which is operating system independent. Several of the
 * more common definitions are provided to enable each user to select a
 * definition which best matches their own experience and understanding of the
 * task concept:
 *
 * - a "dispatchable" unit.
 * - an entity to which the processor is allocated.
 * - an atomic unit of a real-time, multiprocessor system.
 * - single threads of execution which concurrently compete for resources.
 * - a sequence of closely related computations which can execute concurrently
 *   with other computational sequences. 
 *
 * From RTEMS' perspective, a task is the smallest thread of execution which
 * can compete on its own for system resources. A task is manifested by the
 * existence of a task control block (TCB). 
 *
 * @section ClassicTasksSecTaskControlBlock Task Control Block
 *
 * The Task Control Block (TCB) is an RTEMS defined data structure which
 * contains all the information that is pertinent to the execution of a task.
 * During system initialization, RTEMS reserves a TCB for each task configured.
 * A TCB is allocated upon creation of the task and is returned to the TCB free
 * list upon deletion of the task.
 * 
 * The TCB's elements are modified as a result of system calls made by the
 * application in response to external and internal stimuli. TCBs are the only
 * RTEMS internal data structure that can be accessed by an application via
 * user extension routines. The TCB contains a task's name, ID, current
 * priority, current and starting states, execution mode, set of notepad
 * locations, TCB user extension pointer, scheduling control structures, as
 * well as data required by a blocked task.
 *
 * A task's context is stored in the TCB when a task switch occurs. When the
 * task regains control of the processor, its context is restored from the TCB.
 * When a task is restarted, the initial state of the task is restored from the
 * starting context area in the task's TCB.
 *
 * @section ClassicTasksSecTaskStates Task States
 *
 * A task may exist in one of the following five states:
 *
 * - executing - Currently scheduled to the CPU
 * - ready - May be scheduled to the CPU
 * - blocked - Unable to be scheduled to the CPU
 * - dormant - Created task that is not started
 * - non-existent - Uncreated or deleted task
 * 
 * An active task may occupy the executing, ready, blocked or dormant state,
 * otherwise the task is considered non-existent. One or more tasks may be
 * active in the system simultaneously. Multiple tasks communicate,
 * synchronize, and compete for system resources with each other via system
 * calls. The multiple tasks appear to execute in parallel, but actually each
 * is dispatched to the CPU for periods of time determined by the RTEMS
 * scheduling algorithm. The scheduling of a task is based on its current state
 * and priority.
 *
 * @section ClassicTasksSecTaskPriority Task Priority
 *
 * A task's priority determines its importance in relation to the other tasks
 * executing on the same processor. RTEMS supports 255 levels of priority
 * ranging from 1 to 255. The data type rtems_task_priority() is used to store
 * task priorities.
 *
 * Tasks of numerically smaller priority values are more important tasks than
 * tasks of numerically larger priority values. For example, a task at priority
 * level 5 is of higher privilege than a task at priority level 10. There is no
 * limit to the number of tasks assigned to the same priority.
 *
 * Each task has a priority associated with it at all times. The initial value
 * of this priority is assigned at task creation time. The priority of a task
 * may be changed at any subsequent time.
 *
 * Priorities are used by the scheduler to determine which ready task will be
 * allowed to execute. In general, the higher the logical priority of a task,
 * the more likely it is to receive processor execution time.
 *
 * @section ClassicTasksSecTaskMode Task Mode
 *
 * A task's execution mode is a combination of the following four components:
 *
 * - preemption
 * - ASR processing
 * - timeslicing
 * - interrupt level 
 *
 * It is used to modify RTEMS' scheduling process and to alter the execution
 * environment of the task. The data type rtems_task_mode() is used to manage
 * the task execution mode.
 *
 * The preemption component allows a task to determine when control of the
 * processor is relinquished. If preemption is disabled (@c
 * RTEMS_NO_PREEMPT), the task will retain control of the
 * processor as long as it is in the executing state -- even if a higher
 * priority task is made ready. If preemption is enabled (@c RTEMS_PREEMPT)
 * and a higher priority task is made ready, then the processor will be
 * taken away from the current task immediately and given to the higher
 * priority task.
 *
 * The timeslicing component is used by the RTEMS scheduler to determine how
 * the processor is allocated to tasks of equal priority. If timeslicing is
 * enabled (@c RTEMS_TIMESLICE), then RTEMS will limit the amount of time the
 * task can execute before the processor is allocated to another ready task of
 * equal priority. The length of the timeslice is application dependent and
 * specified in the Configuration Table. If timeslicing is disabled (@c
 * RTEMS_NO_TIMESLICE), then the task will be allowed to
 * execute until a task of higher priority is made ready. If @c
 * RTEMS_NO_PREEMPT is selected, then the timeslicing component is ignored by
 * the scheduler.
 *
 * The asynchronous signal processing component is used to determine when
 * received signals are to be processed by the task. If signal processing is
 * enabled (@c RTEMS_ASR), then signals sent to the task will be processed
 * the next time the task executes. If signal processing is disabled (@c
 * RTEMS_NO_ASR), then all signals received by the task will
 * remain posted until signal processing is enabled. This component affects
 * only tasks which have established a routine to process asynchronous signals.
 *
 * The interrupt level component is used to determine which interrupts will be
 * enabled when the task is executing. @c RTEMS_INTERRUPT_LEVEL(n) specifies
 * that the task will execute at interrupt level n.
 *
 * - @ref RTEMS_PREEMPT - enable preemption (default)
 * - @ref RTEMS_NO_PREEMPT - disable preemption
 * - @ref RTEMS_NO_TIMESLICE - disable timeslicing (default)
 * - @ref RTEMS_TIMESLICE - enable timeslicing
 * - @ref RTEMS_ASR - enable ASR processing (default)
 * - @ref RTEMS_NO_ASR - disable ASR processing
 * - @ref RTEMS_INTERRUPT_LEVEL(0) - enable all interrupts (default)
 * - @ref RTEMS_INTERRUPT_LEVEL(n) - execute at interrupt level n
 *
 * The set of default modes may be selected by specifying the @ref
 * RTEMS_DEFAULT_MODES constant.
 *
 * @section ClassicTasksSecAccessingTaskArguments Accessing Task Arguments
 *
 * All RTEMS tasks are invoked with a single argument which is specified when
 * they are started or restarted. The argument is commonly used to communicate
 * startup information to the task. The simplest manner in which to define a
 * task which accesses it argument is:
 *
 * @code
 *  rtems_task user_task(
 *  	rtems_task_argument argument
 *  );
 * @endcode
 *
 * Application tasks requiring more information may view this single argument
 * as an index into an array of parameter blocks.
 *
 * @section ClassicTasksSecFloatingPointConsiderations Floating Point Considerations
 *
 * Creating a task with the @ref RTEMS_FLOATING_POINT attribute flag results in
 * additional memory being allocated for the TCB to store the state of the
 * numeric coprocessor during task switches. This additional memory is NOT
 * allocated for @ref RTEMS_NO_FLOATING_POINT tasks. Saving and restoring the
 * context of a @c RTEMS_FLOATING_POINT task takes longer than that of a @c
 * RTEMS_NO_FLOATING_POINT task because of the relatively large amount of time
 * required for the numeric coprocessor to save or restore its computational
 * state.
 *
 * Since RTEMS was designed specifically for embedded military applications
 * which are floating point intensive, the executive is optimized to avoid
 * unnecessarily saving and restoring the state of the numeric coprocessor. The
 * state of the numeric coprocessor is only saved when a @c
 * RTEMS_FLOATING_POINT task is dispatched and that task was not the last task
 * to utilize the coprocessor. In a system with only one @c
 * RTEMS_FLOATING_POINT task, the state of the numeric coprocessor will never
 * be saved or restored.
 *
 * Although the overhead imposed by @c RTEMS_FLOATING_POINT tasks is minimal,
 * some applications may wish to completely avoid the overhead associated with
 * @c RTEMS_FLOATING_POINT tasks and still utilize a numeric coprocessor. By
 * preventing a task from being preempted while performing a sequence of
 * floating point operations, a @c RTEMS_NO_FLOATING_POINT task can utilize
 * the numeric coprocessor without incurring the overhead of a @c
 * RTEMS_FLOATING_POINT context switch. This approach also avoids the
 * allocation of a floating point context area. However, if this approach is
 * taken by the application designer, NO tasks should be created as @c
 * RTEMS_FLOATING_POINT tasks.  Otherwise, the floating point context will not
 * be correctly maintained because RTEMS assumes that the state of the numeric
 * coprocessor will not be altered by @c RTEMS_NO_FLOATING_POINT tasks.
 *
 * If the supported processor type does not have hardware floating capabilities
 * or a standard numeric coprocessor, RTEMS will not provide built-in support
 * for hardware floating point on that processor. In this case, all tasks are
 * considered @c RTEMS_NO_FLOATING_POINT whether created as @c
 * RTEMS_FLOATING_POINT or @c RTEMS_NO_FLOATING_POINT tasks. A floating point
 * emulation software library must be utilized for floating point operations.
 *
 * On some processors, it is possible to disable the floating point unit
 * dynamically. If this capability is supported by the target processor, then
 * RTEMS will utilize this capability to enable the floating point unit only
 * for tasks which are created with the @c RTEMS_FLOATING_POINT attribute.
 * The consequence of a @c RTEMS_NO_FLOATING_POINT task attempting to access
 * the floating point unit is CPU dependent but will generally result in an
 * exception condition.
 *
 * @section ClassicTasksSecPerTaskVariables Per Task Variables
 *
 * Per task variables are used to support global variables whose value may be
 * unique to a task. After indicating that a variable should be treated as
 * private (i.e. per-task) the task can access and modify the variable, but the
 * modifications will not appear to other tasks, and other tasks' modifications
 * to that variable will not affect the value seen by the task. This is
 * accomplished by saving and restoring the variable's value each time a task
 * switch occurs to or from the calling task.
 *
 * The value seen by other tasks, including those which have not added the
 * variable to their set and are thus accessing the variable as a common
 * location shared among tasks, can not be affected by a task once it has added
 * a variable to its local set. Changes made to the variable by other tasks
 * will not affect the value seen by a task which has added the variable to its
 * private set.
 *
 * This feature can be used when a routine is to be spawned repeatedly as
 * several independent tasks. Although each task will have its own stack, and
 * thus separate stack variables, they will all share the same static and
 * global variables. To make a variable not shareable (i.e. a "global" variable
 * that is specific to a single task), the tasks can call
 * rtems_task_variable_add() to make a separate copy of the variable for each
 * task, but all at the same physical address.
 *
 * Task variables increase the context switch time to and from the tasks that
 * own them so it is desirable to minimize the number of task variables. One
 * efficient method is to have a single task variable that is a pointer to a
 * dynamically allocated structure containing the task's private "global" data.
 *
 * A critical point with per-task variables is that each task must separately
 * request that the same global variable is per-task private.
 *
 * @section ClassicTasksSecBuildingTaskAttributeSet Building a Task Attribute Set
 *
 * In general, an attribute set is built by a bitwise OR of the desired
 * components. The set of valid task attribute components is listed below:
 *
 * - @ref RTEMS_NO_FLOATING_POINT - does not use coprocessor (default)
 * - @ref RTEMS_FLOATING_POINT - uses numeric coprocessor
 * - @ref RTEMS_LOCAL - local task (default)
 * - @ref RTEMS_GLOBAL - global task 
 *
 * Attribute values are specifically designed to be mutually exclusive,
 * therefore bitwise OR and addition operations are equivalent as long as each
 * attribute appears exactly once in the component list. A component listed as
 * a default is not required to appear in the component list, although it is a
 * good programming practice to specify default components. If all defaults are
 * desired, then @ref RTEMS_DEFAULT_ATTRIBUTES should be used.  This example
 * demonstrates the attribute_set parameter needed to create a local task which
 * utilizes the numeric coprocessor. The attribute_set parameter could be @c
 * RTEMS_FLOATING_POINT or @c RTEMS_LOCAL | @c RTEMS_FLOATING_POINT. The
 * attribute_set parameter can be set to @c RTEMS_FLOATING_POINT because @c
 * RTEMS_LOCAL is the default for all created tasks. If the task were global
 * and used the numeric coprocessor, then the attribute_set parameter would be
 * @c RTEMS_GLOBAL | @c RTEMS_FLOATING_POINT.
 *
 * @section ClassicTasksSecBuildingModeAndMask Building a Mode and Mask
 *
 * In general, a mode and its corresponding mask is built by a bitwise OR of
 * the desired components. The set of valid mode constants and each mode's
 * corresponding mask constant is listed below:
 *
 * <table>
 * <tr><th>Mode Constant</th><th>Mask Constant</th><th>Description</th></tr>
 * <tr><td>@ref RTEMS_PREEMPT</td><td>@ref RTEMS_PREEMPT_MASK</td><td>enables preemption</td></tr>
 * <tr><td>@ref RTEMS_NO_PREEMPT</td><td>@ref RTEMS_PREEMPT_MASK</td><td>disables preemption</td></tr>
 * <tr><td>@ref RTEMS_NO_TIMESLICE</td><td>@ref RTEMS_TIMESLICE_MASK</td><td>disables timeslicing</td></tr>
 * <tr><td>@ref RTEMS_TIMESLICE</td><td>@ref RTEMS_TIMESLICE_MASK</td><td>enables timeslicing</td></tr>
 * <tr><td>@ref RTEMS_ASR</td><td>@ref RTEMS_ASR_MASK</td><td>enables ASR processing</td></tr>
 * <tr><td>@ref RTEMS_NO_ASR</td><td>@ref RTEMS_ASR_MASK</td><td>disables ASR processing</td></tr>
 * <tr><td>@ref RTEMS_INTERRUPT_LEVEL(0)</td><td>@ref RTEMS_INTERRUPT_MASK</td><td>enables all interrupts</td></tr>
 * <tr><td>@ref RTEMS_INTERRUPT_LEVEL(n)</td><td>@ref RTEMS_INTERRUPT_MASK</td><td>sets interrupts level n</td></tr>
 * </table>
 * 
 * Mode values are specifically designed to be mutually exclusive, therefore
 * bitwise OR and addition operations are equivalent as long as each mode
 * appears exactly once in the component list. A mode component listed as a
 * default is not required to appear in the mode component list, although it is
 * a good programming practice to specify default components. If all defaults
 * are desired, the mode @ref RTEMS_DEFAULT_MODES and the mask @ref
 * RTEMS_ALL_MODE_MASKS should be used.
 * 
 * The following example demonstrates the mode and mask parameters used with
 * the rtems_task_mode() directive to place a task at interrupt level 3 and
 * make it non-preemptible. The mode should be set to @c
 * RTEMS_INTERRUPT_LEVEL(3) | @c RTEMS_NO_PREEMPT to indicate the desired
 * preemption mode and interrupt level, while the mask parameter should be set
 * to @c RTEMS_INTERRUPT_MASK | @c RTEMS_PREEMPT_MASK to indicate that
 * the calling task's interrupt level and preemption mode are being altered.
 */

 /**
  * @defgroup LocalPackages Local Packages
  *
  * @brief Local packages.
  */
