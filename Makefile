CC       = gcc
CXX      = g++
CFLAGS   = -Wall -Wextra -Iinclude
CXXFLAGS = -Wall -Wextra -Iinclude
LDFLAGS  = -lpthread
FUSE_FLAGS = `pkg-config fuse --cflags --libs`

# ============================================================
# izvorni fajlovi
# ============================================================
CORE_SRCS = src/gf.c src/aes.c
MODES_C   = src/aes_modes.c
MODES_CPP = src/aes_modes.cpp
FUSE_SRC  = src/aes_fuse.c

# ============================================================
# default — sve
# ============================================================
all: aes_fs test_basic test_modes

# ============================================================
# FUSE fajl sistem (64-bit, C)
# ============================================================
aes_fs: $(CORE_SRCS) $(MODES_C) $(FUSE_SRC)
	$(CC) $(CFLAGS) $(CORE_SRCS) $(MODES_C) $(FUSE_SRC) -o aes_fs $(FUSE_FLAGS)

# ============================================================
# test AES core (C)
# ============================================================
test_basic: $(CORE_SRCS) test/test_encr_decr.c
	$(CC) $(CFLAGS) $(CORE_SRCS) test/test_encr_decr.c -o test_basic

# ============================================================
# test modova (C + C++ zbog threadova)
# ============================================================
test_modes: $(CORE_SRCS) $(MODES_C) $(MODES_CPP) test/test_modes.cpp
	$(CXX) $(CXXFLAGS) $(CORE_SRCS) $(MODES_C) $(MODES_CPP) test/test_modes.cpp -o test_modes $(LDFLAGS)

# ============================================================
# pokretanje testova
# ============================================================
test: test_basic test_modes
	./test_basic
	./test_modes

# ============================================================
# montiranje i demontiranje FUSE-a
# ============================================================
mount: aes_fs
	mkdir -p /tmp/aes_storage /tmp/aes_mount
	fusermount -u /tmp/aes_mount 2>/dev/null || true
	./aes_fs /tmp/aes_storage /tmp/aes_mount

umount:
	fusermount -u /tmp/aes_mount

# ============================================================
# brisanje kompajliranih fajlova
# ============================================================
clean:
	rm -f aes_fs test_basic test_modes aes_demo

# ============================================================
# demo — enkriptuje README i pokazuje transparentnu enkripciju
# ============================================================
demo: aes_fs
	@echo "=== Priprema ==="
	@fusermount -u /tmp/aes_mount 2>/dev/null || true
	@mkdir -p /tmp/aes_storage /tmp/aes_mount
	@rm -rf /tmp/aes_storage/* /tmp/aes_mount/*
	@echo ""
	@echo "****Montiranje FUSE fajl sistema****"
	@./aes_fs /tmp/aes_storage /tmp/aes_mount
	@sleep 1
	@echo ""
	@echo "____Kopiranje README u enkriptovani fajl sistem ____"
	@cp README.md /tmp/aes_mount/README.md
	@echo "README kopiran."
	@echo ""
	@echo "=== Kako izgleda na disku? (ENKRIPTOVANO) ==="
	@xxd /tmp/aes_storage/README.md | head -10
	@echo ""
	@echo "=== Kako izgleda kroz fajl sistem? (DEKRIPTOVANO) ==="
	@cat /tmp/aes_mount/README.md
	@echo ""
	@echo "=== FUSE je i dalje montiran. Pokreni 'make enddemo' za demontiranje. ==="

# ============================================================
# enddemo — demontira FUSE i cisti nakon demonstracije
# ============================================================
enddemo:
	@echo "=== Demontiranje FUSE fajl sistema ==="
	@fusermount -u /tmp/aes_mount 2>/dev/null || true
	@echo "Gotovo."

# ============================================================
# demo_basic — konzolna aplikacija koja pokazuje sve AES varijante
# ============================================================
demo_basic: src/main.cpp $(CORE_SRCS) $(MODES_C) $(MODES_CPP)
	$(CXX) $(CXXFLAGS) src/main.cpp $(CORE_SRCS) $(MODES_C) $(MODES_CPP) -o aes_demo $(LDFLAGS)
	./aes_demo

.PHONY: all test mount umount demo enddemo demo_basic clean