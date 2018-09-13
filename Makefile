COMPILER = g++ -std=c++17
LINKER = g++
INCLUDE = -I/usr/local/include -I$(CURDIR)/headers
CFLAGS = -c -g
LIBS = -L/usr/local/lib -L$(CURDIR)/bin

OBJECTS = $(patsubst $(CURDIR)/sources/%.cpp, $(CURDIR)/bin/%.o, \
	$(wildcard $(CURDIR)/sources/*.cpp))
DEPS = $(patsubst $(CURDIR)/bin/%.o, $(CURDIR)/Dependencies/%.d, $(OBJECTS))
TESTS = $(patsubst $(CURDIR)/sources/%_test.cpp, $(CURDIR)/bin/Test_%.out, \
	$(wildcard $(CURDIR)/sources/*_test.cpp))
EXEC = $(patsubst $(CURDIR)/sources/%_main.cpp, $(CURDIR)/bin/%.out, \
	$(wildcard $(CURDIR)/sources/*_main.cpp))
TARGETS = $(TESTS) $(EXEC)

DOCS = $(patsubst $(CURDIR)/Docs/%.tex, $(CURDIR)/Docs/%.pdf, \
	$(wildcard $(CURDIR)/Docs/*.tex))

all: $(TARGETS)
	@echo Compilation finished successfuly. All target ready to use!!!!!!!

include $(DEPS)

$(CURDIR)/Dependencies/%.d: $(CURDIR)/sources/%.cpp
	$(COMPILER) $(INCLUDE) -MM -MF$@ -MT$(CURDIR)/bin/$*.o $<

$(CURDIR)/bin/%.o: $(CURDIR)/sources/%.cpp
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o$@ $<

$(CURDIR)/bin/Test_SQLiteManager.out: $(CURDIR)/bin/SQLiteManager_test.o \
	$(CURDIR)/bin/SQLiteManager.o
	$(COMPILER) $(LIBS) -lboost_unit_test_framework -lboost_filesystem \
	-lboost_system -lsqlite3 -o$@ $^

$(CURDIR)/bin/Test_AVConnection.out: $(CURDIR)/bin/AVConnection_test.o\
$(CURDIR)/bin/AVConnection.o $(CURDIR)/bin/JSONParser.o
	$(COMPILER) $(LIBS) -lboost_unit_test_framework -lcurl -o$@ $^

$(CURDIR)/bin/Test_JSONParser.out: $(CURDIR)/bin/JSONParser_test.o \
	$(CURDIR)/bin/JSONParser.o
	$(COMPILER) $(LIBS) -lboost_unit_test_framework -o$@ $^

$(CURDIR)/bin/libalphavantage.a: $(CURDIR)/bin/AVConnection.o \
	$(CURDIR)/bin/JSONParser.o
	rm -f $@
	ar -rcs $@ $^

$(CURDIR)/bin/AlphaVantage.out: $(CURDIR)/bin/libalphavantage.a \
	$(CURDIR)/bin/AlphaVantage_main.o
	$(COMPILER) $^ $(LIBS) -lboost_program_options -lalphavantage -lcurl -o$@

.PHONY: clean
clean:
	rm -f $(CURDIR)/bin/*.o $(CURDIR)/bin/*.out $(CURDIR)/Dependencies/*.d

.PHONY: print_variables
print_variables: 
	@echo "The current project has the following targets:"
	@echo $(TARGETS)
	@echo "The current project has the following object files:"
	@echo $(OBJECTS)
	@echo "Project documentation follows:"
	@echo $(DOCS)

.PHONY: compile_docs
compile_docs: $(DOCS)
	@echo "All documents are ready to read..."

$(CURDIR)/Docs/%.pdf: $(CURDIR)/Docs/%.tex
	pdflatex -output-directory Docs $<

.PHONY: clean_docs
clean_docs:
	rm -f $(CURDIR)/Docs/*.pdf

.PHONY: run_tests
run_tests:
	$(foreach test, $(TESTS), echo "Running $(test)..."; $(test) \
	--report_level=detailed;)

.PHONY: run_ctags
run_ctags:
	@echo "Updating ctags..."
	  ctags -R

.PHONY: edit_project
edit_project:
	@echo "Open project files in vim..."
	vim Makefile $(foreach stem, JSON AV SQL Alpha, ./headers/$(stem)* \
	./sources/$(stem)*)

