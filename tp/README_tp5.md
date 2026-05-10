# Scheduling

The objective of this lab is to put into practice the concepts presented in the course the course, the notion of multi-activities and the different types of schedulers.

The course of the course is divided into three parts. The first part consists in validating the knowledge acquired in the previous exercise. You are asked to find a bug in the code provided in relation to the management of the execution stack. The second part is dedicated to the manipulation of activities via the cpu_context_switch(ctxt *, ctxt *), and create_kernel_thread(char *, (kernel_thread_start_routine_t) *, void *). This second part is about manipulating two activities that display a string on the screen. Finally, the last part will focus on the Thread class.

Except for the last part, all the code to be written or modified will be in the main.cpp file.

**Import the Git project of the practical work :**

1. Go to View->Command Palelette menu or type "Shift + Control + p" => Enter.
2. Enter 'clone' and run the command git: clone
3. Enter URL **https://gitlab-df.imt-atlantique.fr/fila2-os/TP5.git**
4. Save the repository in Documents/sextant
5. At the message appearing at the bottom right, click on "Add to Workspace"

![Capture16](images/Capture16.PNG)

## 1. Question on practical work n-1

### 1.1 buffer overflow ! (60 minutes)

A buffer overflow flaw has been inserted into the code. Once found you will need to analyse the causes and understand how it was implemented. To help you, the buffer overflow is located in the endlessfunc(Ecran *).

After understanding the mechanism, move the declaration of the variable tmp variable below piRet and diff, compile and relaunch.... what happens?

Before proceeding to question 2, uncomment line 269 (while(true)) of the main function.

## 2. Scheduling

### 2.1 Back to DOS : coroutining (30 minutes)

For this question, we have developed two functions Hello1 and Hello2 which display on the screen a character string passed in parameter. Using the cpu_context_switch function in the extant\ordonnancements\cpu_context_switch.S file, make sure that the message displayed by the two hello's are correctly interleaved. That is, that "Hello World" is displayed on the screen. An example of using the cpu_context_switch function is given in the void print_hello_world ().

![MSDOS](images/MSDOS.png)

This direct cooperative scheduling system was the only one available in the DOS operating system.

Before moving on to question 3, comment on line 279 (while(true));

### 2.2 core_routine (15 minutes)

Study the codes of the file sextant\ordonnancements\cpu_context.cpp. What is the purpose of the 'core_routine' function?.

### 2.3 Back to Windows 3.11 : Cooperative (30 minutes)

In the first questions, the activities were in a direct cooperative mode. The activity associated with Hello1 made direct use of the context of the activity associated with Hello2. A modern operating system must allow for the dynamic creation of activities. The operating system must provide a function for activities to simply release the processor without having to designate the next activity to be executed. This is the purpose of the thread_yield() function. The purpose of this method is to release the processor and hand over to the scheduler. The latter will choose one of the activities in a ready state. All the code for cooperative scheduling can be found in the eponymous directory.

Study the code of the thr_hello1 and thr_hello2 functions. Notice that the threads are created and cooperate via the thread_yield() method. Apart from the message in line 297, nothing is displayed. Why not? Solve the problem.

One solution to this problem is that the message in line 297 is no longer displayed. If you are in this situation, please explain why ?

![windows-311](images/windows-311.jpg)

You have just implemented/used a cooperative scheduler such as the one offered in Windows versions 1 to 3.11.

Before moving on to question 5, comment on line 299 (while(true)); and the code added to main to solve the problem.

### 2.4 Back to Windows 95 : Preemptive (30 minutes)

There are a number of disadvantages to using a cooperative scheduler. Among other things, if one of the activities is unwilling or unable to release the processor, the other activities get stuck in the ready state. This problem existed This problem existed in the Mac OS 1 to 9 and Windows 1 to 3.11 operating systems. If an application was not working (crashed), the whole system was blocked.

There are two possible solutions. The first is to implement a Control+Alt+Del function, the second is to develop a system to preempt the processor to an activity. It is the second solution that we have chosen.

Transform your code to accept preemptive scheduling. The solution is a single line of code to add to the main. First we will cause the activity to change by pressing a key (any key on the keyboard). a key (any key on the keyboard). In a second step, this change should be done every x milliseconds. every x milliseconds.

**Indixes:**

* Find the sched_clk function in the preemptive directory
* IRQ_TIMER and/or IRQ_KEYBOARD

After finding the solution, the screen should display the characters of the four strings. The display of the characters should interleave randomly. These strings are two "Hlowrd" and two "el ol".

Why do they intertwine randomly ?

Why four channels and not two ?

![Win95](images/Win95.jpg)

Bravo, you have just implemented/used a preemptive scheduler such as the one implemented in windows 95.

## 3. The Thread class

### 3.1 Implementation (75 minutes)

Implement a Thread class in the directory to be created sextant/Activity. The objective of this class is to provide the same functionality as the Java "Thread" class.

```c
class PrimeThread extends Thread {
         long minPrime;
         PrimeThread(long minPrime) {
             this.minPrime = minPrime;
         }
 
         public void run() {
             // compute primes larger than minPrime
              . . .
         }
     }
```

Then to be able to use it as follows :

```c
PrimeThread p(143);
	p.start();
```

## End of the session