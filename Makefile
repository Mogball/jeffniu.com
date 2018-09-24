EXE=website
BIN=bin

$(BIN)/$(EXE): $(BIN)/Makefile
	@make -C $(BIN) -j4 --no-print-directory

$(BIN)/Makefile: $(BIN) CMakeLists.txt
	@cd $(BIN) && cmake ../

$(BIN):
	mkdir -p $(BIN)

.PHONY: run port

run:
	@$(BIN)/$(EXE)
