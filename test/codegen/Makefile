#---------------------------------------------------------------------------------------------------
# SnuPL/2 Compiler Driver
#
ROOT=../../snuplc
SnuPL2_RUNTIME=snupl

TARGET=x86-64
TARGET_BITS=64

%: %.mod
	$(ROOT)/snuplc --target $(TARGET) --ast --tac $<
	gcc -m$(TARGET_BITS) -L$(ROOT)/rte/$(TARGET) -o $@ $<.s -l$(SnuPL2_RUNTIME)

all:
	@for i in *.mod; do \
		make $${i%%.mod}; \
	done

clean:
	@for i in *.mod; do \
		rm -f $${i%%.mod} $${i}.ast* $${i}.tac* $${i}.dot* $${i}.s; \
	done
