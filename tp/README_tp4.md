# The Activity or Thread

The course of the tutorial is subdivided into three parts. The first part consists in validating the knowledge acquired in TP3. You are asked to find a bug in the code provided in relation to the management of hardware interrupts. The second part is dedicated to the manipulation of the stack. The aim is to display then modify the return address of a function present in the stack. Finally the last part will focus on the management of the heap. We ask you to implement a mini-manager of dynamic memory

**Importing the Git project from the practical work :**

1. Go to View->Command Palelette menu or type "Shift + Control + p" => Enter.
2. Enter 'clone' and run the command git: clone
3. Enter URL **https://gitlab-df.imt-atlantique.fr/fila2-os/TP4.git**
4. Save the repository in Documents/sextant
5. At the message appearing at the bottom right, click on "Add to Workspace"

![Capture16](images/Capture16.PNG)

## 1. Question on practical work n-1

### 1.1 Our game no longer works! (3 minutes)

In particular, the rackets no longer move up and down when we press the keys that are associated with these movements on the keyboard. Why ?

### 1.2 It was so simple (5 minutes)

Modify the code to return to the operation of the previous TP. This code boils down to adding a line and deleting a line in the Sextant_main function of the main.cpp file.

### 1.3 The reset key (5 minutes)

Once the keyboard is restarted, we ask you to complete the reboot function in the handler_clavier.cpp file located in the directory : sextant/interruptions/handler.

This function is called when you press the q key. We want our machine to reboot every time we press the q key. To do this, just write 0xfe to the keyboard microcontroller at port 0x64. Test.

## 2. The stack, principle of handling

To continue to the following questions, put this code in comment :

```c
while(true) {
	h.afficheHeure();
	if (clavier.testChar()) jeux.start(); 
}
```

### 2.1 Highlighting the reverse addressing of the stack (30 minutes)

In main.cpp we have included two functions that do not perform any special processing, func and func2. These will be used to support our discussion of stack management. First, study them.

Display on the screen the memory addresses of the parameters a and b of the function func and c and d of func2. What are the variables whose addresses are at the extremes (the lowest and the highest). Comment on this.

![Pile](images/Pile.jpg)

### 2.2 The return address (30 minutes)

Now display the return address of the func function (4 bytes from the address of parameter a). To answer this question, see the previous figure.

**Note:**

Reminders on pointer arithmetic: if you declare an int *p; then if you add 1 to p it is equivalent to p=p+siezof(int). In our case, the integers are encoded on 4 bytes. If p is 0x02, then after instruction p=p+4, p will be 0x12 (18 in decimal).

### 2.3 Binary code of the return address (15 minutes)

You now have the return address of func. We are going to look for in the binary code for the instruction associated with this address. To do this, disassemble the binary code sextant.elf with the help of objectdump (redirect the output using " > resultat.text").

The code displayed is formatted in columns. Each of the lines contains a certain amount of a certain amount of information. Detail the information in the following line

```
  20102f:       e8 38 12 00 00          call   20226c <_ZN5Ecran11afficherMotEPKc7Couleur>
```

What does the reference to <_ZN5Ecran11afficherMotEPKc7Couleur> ? How to get clearer information information ?

**Incide:**

demangling.

### 2.4 Let's play jumping jacks: find the sheep (15 minutes)

In main we have the call to func surrounded by two assignments to the variable i. At the end of the execution of func, i is assigned to 1. We will skip this instruction. The first step is to find the address of this instruction in the binary of sextant.elf.

```c
i=0;
    func(1,2,&ecran); 
    i=1;
```

**Incide:**

To find the assembly instructions that assign i to the value 1, help you to find the address found in the previous question, deduce the address of the instruction after the assignment of i to 1.

## 3. Memory Manager

### 3.1 The allocation (30 minutes)

Develop a primitive heap manager. To do this, override the new() operator, then complete a first handler located in sextant/memoire/memoire.cpp

The principle is as follows. A pointer points to the start address of the available heap (initialized by the mem_setup method). At each allocation request, return the address of this pointer, then shift this pointer by n bytes. The n bytes correspond to the requested memory size passed as a parameter to the getmem().

### 3.2 simple deallocation (60 minutes)

We will implement the delete. We do not ask you to implement a chained management of free blocks, but simply, at first, to develop a method freemem(int address,int size) writing zeros in the freed memory areas.

### 3.3 clever deallocation (60 minutes)

When you override the delete operator, you will only get the address of the memory area to be freed. Find a way to retrieve the size associated with the the object/lastrucutre to be deallocated.

The following scenario should return an error :

```c
void main()
{
Ecran ∗ec ;
ec= new Ecran ;
ec−>AfficherMot("Test1") ;
delete ec ;
ec−>AfficherMot("Test2") ;
}
```

## End of the session