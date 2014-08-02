CC := gcc
STRIP := strip
CFLAGS := -O3 -std=gnu99 -Wall -Werror
LDFLAGS :=

progs := rdrand
deps  :=$(progs:=_deps)
asms  :=$(progs:=_asm)

rdrand_src     := rdrand.c

.PHONY: all clean depend
all: $(progs)

define prog_deps
define $(1)_deps
$($(1)_src:%.c=%.o)
endef
define $(1)_asm
$($(1)_src:%.c=%.s)
endef
$(1): $$($(1)_src:%.c=%.o) $$($(1)_lib)
$(1)_asm:  $$($(1)_src:%.c=%.s) $$($(1)_lib)
endef
$(foreach prog,$(progs),$(eval $(call prog_deps,$(prog))))

assembly: $(asms)

%.s: %.c Makefile
	$(CC) -S $(CFLAGS) $<

%.o: %.c Makefile
	$(CC) -c -o $@ $(CFLAGS) $<

${progs}:
	$(CC) ${CFLAGS} -o $@ $+ ${LDFLAGS}

clean:
	$(RM) -r $(foreach dep,$(deps),$($(dep))) $(foreach asm,$(asms),$($(asm))) $(progs)
