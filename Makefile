# Makefile — CSE-PAR: parallelize a whole over CSE-IO + DSA. Only slate types cross.
#
# Build the deps first: make -C ../CardinalSlate lib && make -C ../CSE-DSA lib && make -C ../CSE-IO lib

SPINE ?= ../CardinalSlate
DSA   ?= ../CSE-DSA
IO    ?= ../CSE-IO
CC    ?= clang
CFLAGS := -std=c11 -Iinclude -I$(IO)/include -I$(DSA)/include -I$(SPINE)/include -O2 -Wall -Wextra

OUT     := build
HDRS    := $(wildcard include/cse/par/*.h) include/cse/par.h
SRCS    := $(wildcard src/*.c)
OBJS    := $(patsubst src/%.c,$(OUT)/%.o,$(SRCS))
DEPLIBS := $(IO)/build/libcse-io.a $(DSA)/build/libcse-dsa.a

.PHONY: all check clean lib
all: check lib

$(OUT):
	@mkdir -p $(OUT)

$(OUT)/types.stamp: $(HDRS) $(SRCS) | $(OUT)
	@bad=$$(grep -rnE '\b(int|long|short|size_t|unsigned|char|bool|float|double)\b|void[[:space:]]*\*|stdint' include src 2>/dev/null || true); \
	  if [ -n "$$bad" ]; then printf "  %-10s C TYPE FOUND\n" "types:"; printf '%s\n' "$$bad" | sed 's/^/    /'; exit 1; \
	  else printf "  %-10s only slate\n" "types:"; fi; touch $@

$(OUT)/standalone.stamp: $(HDRS) | $(OUT)
	@for h in $(HDRS); do \
	  rel=$${h#include/}; \
	  printf '#include "%s"\nint main(void){return 0;}\n' "$$rel" > $(OUT)/one.c; \
	  $(CC) $(CFLAGS) -fsyntax-only $(OUT)/one.c || exit 1; \
	done; touch $@

$(OUT)/%.o: src/%.c $(HDRS) | $(OUT)
	@$(CC) $(CFLAGS) -c $< -o $@

lib: $(OUT)/libcse-par.a
$(OUT)/libcse-par.a: $(OBJS) | $(OUT)
	@ar rcs $@ $(OBJS)

$(OUT)/test_par: tests/par.c $(OBJS) | $(OUT)
	@$(CC) $(CFLAGS) tests/par.c $(OBJS) $(DEPLIBS) -o $@

check: $(OUT)/types.stamp $(OUT)/standalone.stamp $(OUT)/test_par
	@echo "== cse-par =="; out=$$($(OUT)/test_par 2>&1); st=$$?; \
	  if [ $$st -ne 0 ] || printf '%s' "$$out" | grep -q FAIL; then printf '%s\n' "$$out" | sed 's/^/  /'; exit 1; \
	  else printf "  %-10s %s\n" "par:" "$$(printf '%s' "$$out" | tail -1)"; echo "== ALL PASS =="; fi

clean:
	@rm -rf $(OUT)
