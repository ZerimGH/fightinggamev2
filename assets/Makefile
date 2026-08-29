FILE2C = ./file2c/file2c
ASSETS = $(wildcard *.png)
TARGET_C = ../src/render/assets.c
TARGET_H = ../src/render/assets.h

.PHONY: all
all: $(TARGET_C) $(TARGET_H)

.PHONY: clean
clean:
	rm -f $(TARGET_C) $(TARGET_H)
	make -C ./file2c clean

$(FILE2C):
	make -C ./file2c all

$(TARGET_C): $(FILE2C)
	$(FILE2C) $(ASSETS) > $(TARGET_C)

$(TARGET_H): $(FILE2C)
	echo '#ifndef ASSETS_H' > $(TARGET_H)
	echo '#define ASSETS_H' >> $(TARGET_H)
	$(FILE2C) -h $(ASSETS) >> $(TARGET_H)
	echo '#endif /* ASSETS_H */' >> $(TARGET_H)
