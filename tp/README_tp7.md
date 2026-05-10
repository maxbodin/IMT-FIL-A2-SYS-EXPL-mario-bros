# Memory management, the heap

The course of the practical work is divided into two parts. The first part consists of validating the knowledge acquired in the previous exercise. You are asked to develop an application to calculate the binary value of a number via the synchronisation of several activities. The second part is devoted to the manipulation and implementation of memory management mechanisms.

Attention: You are asked to configure qemu to initialise the size of the memory available on the PC to 4 MB. available memory on the PC to 4 MB. To do this, add or at the beginning of the qemu startup options : ** -m 4 **.

**Importing the Git project from the practical work :**

1. Go to View->Command Palelette menu or type "Shift + Control + p" => Enter.
2. Enter 'clone' and run the command git: clone
3. Enter URL **https://gitlab-df.imt-atlantique.fr/fila2-os/TP7.git**
4. Save the repository in Documents/sextant
5. At the message appearing at the bottom right, click on "Add to Workspace"

![Capture16](images/Capture16.PNG)

## 1. Question on practical work n-1

You are asked to complete the Pipeline program, located in the Applications/Pipeline directory, which converts integer numbers of values less than 256 from base 10 to base 2. The number to be converted is stored in the globalCounter variable. The calculation of a task (or activity/thread) is simply to display a 0 or 1 depending on whether the number contained in the globalCounter is divisible by two or not. To display the binary value of a number, we must launch as many tasks as the number's bits. We assume that the number to convert will be less than 256, We therefore need 8 spots to display the binary value of the number (in fact 2^8=256). The tasks are identified by a unique identifier (a number between 0 and 7). Once the task n has displayed 0 or 1 it divides by 2 globalCounter. (the operation globalCounter=globalCounter >> 1; allows to divide by 2 globalCounter by shifting a bit).

### 1.1 Disordered pipeline (15 minutes)

For the first part of the question, we do not impose a strict order. Each of the activities can start the calculation as soon as it can. (Hint: only one semaphore is needed). In other words, task n can run before task n-1. However, only one task must be active at the same time.

### 1.2 Ordered pipeline (15 minutes)

In the second part, we ask you to adapt your code to respect a predetermined order of execution of the tasks. In this part, task n should only be executed after task n-1 (Hint: 8 semaphores).

## 2. Memory Management

### 2.1 Memoire.cpp (15 minutes)

Study the code in the file Memory.cpp. This architecture allows to replace a memory manager by another one by redefining the type of the variable uneMemory. This code respects the Singleton design pattern. Study the code.

We remind you that in software engineering, the singleton is a design pattern whose purpose is to restrict the instantiation of a class to a single object. It is used when exactly one object is needed to coordinate operations in a system such as the operations in a system, such as the memory manager (which must be unique in the sextant system). The singleton is implemented by writing a class containing a method that creates an instance only if one does not yet exist. Otherwise it returns a reference to the object that already exists.

### 2.2 Pion memory (30 minutes)

We ask you to reimplement the memory manager code pion in this architecture (Singleton model). You can of course use the codes already implemented in the previous tutorials.

**Reminder:**

A pointer (void *debut;) points to the start address of the available heap (initialized by the mem_setup method). At each request to allocate n bytes, you must return the address of this pointer, then shift this pointer by n bytes.

For memory release, we do not ask you to implement a chained management of free blocks, but simply, as a first step, to develop a method freemem(int adresse,int taille) writing zeros in the freed memory areas. Use the memset function in the op_memoire.cpp file.

The main drawback of the freemem method is that it cannot be used by the the C++ delete operator. Indeed, the latter does not have the memory size to be deallocated. The next question allows us to overcome this limitation.

### 2.3 Pion memory "clever" (60 minutes)

We described in class an algorithm for allocating memory on the heap and then freeing it with only the address. The idea is, in the malloc() function, to use the first bytes of the area to save the size of the allocated area. Then, in the free() function, retrieve the size by going to read in this area.

Implement the malloc and free code in the memoirepion.cpp file.

### 2.4 Memory list (60 minutes)

The code detailed in the memoirepion.cpp file allows you to overwrite the allocated areas with zeros, but does not allow them to be reused. An improvement consists of putting the released areas back into the allocable area part. This is what the code of the memoirelist.cpp file proposes to do.

The code presented is used to dynamically manage a list of free memory zones. Detail on paper the algorithmic of this code.

In practice, this does not work because the code lacks the part associated with managing the size of the area to deallocate. Correct using the previous question.

### 2.5 Memory Block (60 minutes)

As you might expect, the latest memory manager induces severe memory fragmentation. Explain why.

Memory managers typically use virtual addressing mechanisms to handle this fragmentation. Virtual addressing is based on a uniform division of the memory into of the memory by fixed size blocks. Before studying virtual addressing mechanisms virtual addressing mechanisms, we need a fixed size block memory manager.

The principle remains very simple. The memory is divided into blocks of fixed size K. If the size of a memory request is smaller than K, the manager returns the address of the first free block, and marks this block as occupied. If the memory size is greater than K, the handler returns an error. On memory release, the block is reset by writing zero, and marked as free. Initially, all blocks are marked free.

The mechanism for managing the occupation of blocks (free or occupied) can be implemented in different ways. We leave it to you to choose this implementation, but we advise you to validate your algorithm with your tutor before any implementation.

## End of the session