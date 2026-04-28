OS=$(shell uname)
COMPILATEUR=CPP_$(OS)
LIEUR=LD_$(OS)
COMPILATEUR_OPTION=COMPOP_$(OS)
LIEUR_OPTION=LIEUR_$(OS)

#-------------------
##Pour Mac
CPP_Darwin=i386-elf-g++-4.3.2
LD_Darwin=i386-elf-ld

#-------------------
##Pour Linux
CPP_Linux=g++
LD_Linux=ld
COMPOP_Linux=-fno-stack-protector -m32 -fpermissive
LIEUR_Linux=-m elf_i386
QEMU=qemu-system-i386 

#-------------------
##POUR WINDOWS
CPP_WindowsNT=g++
LD_WindowsNT=ld
 
#-------------------
## Partie commune a toutes les configurations

CPPFLAGS  = -gdwarf-2 -g3 -Wall -fno-builtin -fno-rtti -fno-exceptions -nostdinc --std=c++11 $($(COMPILATEUR_OPTION))
LDFLAGS = --warn-common -nostdlib $($(LIEUR_OPTION))

PWD :=.
DELE = rm -rf
MV = mv -f


KERNEL_OBJ   = sextant.elf

OBJECTSNAMES= main multiboot fonctionsES irq_wrappers i8259 idt irq Ecran Clavier timer handler_tic handler_clavier memoire vga shared_palette peashooter_sprite peashooter_bullet_sprite grass_tile_sprite Entity Bullet Peashooter Grid Tile PlantsVsZombies


OBJECTS=$(patsubst %,build/all-o/%.o,$(OBJECTSNAMES))					  		

#variable pour demander a make de chercher les dependances dans n'importe quel repertoire jusqu'à 3 rep de profondeur :
VPATH=$(wildcard *):$(wildcard */*):$(wildcard */*/*)

# les target all et clean ne sont pas "constructibles" mais appellent des recettes :
.PHONY:all clean run show


# Main target
all: $(KERNEL_OBJ)


OBJ_FILES = $(wildcard build/all-o/*.o)

$(KERNEL_OBJ): $(OBJECTS)
	@echo 'Votre compilateur $($(COMPILATEUR)) et votre lieur $($(LIEUR))'
	$($(LIEUR)) $(LDFLAGS) -T ./support/sextant.lds -o build/boot/$@ $(OBJECTS)

#compiler tout .cpp dans les repertoires de sources en .o dans le build/all-o . On cree d'abord le repertoire build/all-o s'il n'existe pas.
build/all-o/%.o:%.cpp %.h
	$($(COMPILATEUR)) -I$(PWD) -c $< $(CPPFLAGS) -o $@

# la meme que precedement, si pas de .h correspondant on compile quand meme.
build/all-o/%.o:%.cpp
	$($(COMPILATEUR)) -I$(PWD) -c $< $(CPPFLAGS) -o $@
	
#meme regle que precedente pour les fichiers .s
build/all-o/%.o: %.S
	$($(COMPILATEUR)) -I$(PWD)  -c $< $(CPPFLAGS) -DASM_SOURCE=1 -o $@



# Clean directory
clean:
	$(DELE) build/all-o/*.o
	$(DELE) *.s
	$(DELE) build/boot/*.elf

run: $(KERNEL_OBJ)
	$(QEMU) -display curses -net nic,model=ne2k_isa -net user,tftp=./build/boot -cdrom ./build/boot/grub.iso

run_gui: $(KERNEL_OBJ)
	$(QEMU) -net nic,model=ne2k_isa -net user,tftp=./build/boot -cdrom ./build/boot/grub.iso -monitor stdio

debug: $(KERNEL_OBJ)
	$(QEMU) -display curses -S -s  -net nic,model=ne2k_isa -net user,tftp=./build/boot -cdrom ./build/boot/grub.iso

debug_gui: $(KERNEL_OBJ)
	$(QEMU) -S -s  -net nic,model=ne2k_isa -net user,tftp=./build/boot -cdrom ./build/boot/grub.iso -monitor stdio

show:
	@echo "objects:$(OBJECTS)"
	@echo "VPATHS:$(VPATH)"
	@echo "compilateur:"$($(COMPILATEUR))" ; lieur:"$($(LIEUR))
 
	
