# GNU make手册：http://www.gnu.org/software/make/manual/make.html
# ************ 遇到不明白的地方请google以及阅读手册 *************

# 编译器设定和编译选项
CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -std=c99

# 编译目标：src目录下的所有.c文件
CFILES = $(shell find ./ -name "*.c")
OBJS = $(CFILES:.c=.o)
LFILE = $(shell find ./ -name "*.l")
YFILE = $(shell find ./ -name "*.y")
LFC = $(shell find ./ -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find ./ -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)

parser: syntax $(filter-out $(LFO),$(OBJS))
	$(CC) -o parser $(filter-out $(LFO),$(OBJS)) -ll -ly

syntax: lexical syntax-c
	$(CC) -c $(YFC) -o $(YFO)

lexical: $(LFILE)
	$(FLEX) -o $(LFC) $(LFILE)

syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d -v $(YFILE)

-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: clean test
test:
	./parser ../Test/pretest/1.txt;
	./parser ../Test/pretest/2.txt;
	./parser ../Test/pretest/3.txt;
	./parser ../Test/pretest/4.txt;
	./parser ../Test/pretest/5.txt;
	./parser ../Test/pretest/6.txt;
	./parser ../Test/pretest/7.txt;
	./parser ../Test/pretest/8.txt;
	./parser ../Test/pretest/9.txt;
	./parser ../Test/pretest/10.txt;
	./parser ../Test/pretest/11.txt;
	./parser ../Test/pretest/12.txt;
	./parser ../Test/pretest/13.txt;
	./parser ../Test/pretest/14.txt;
	./parser ../Test/pretest/15.txt;
	./parser ../Test/pretest/16.txt;
	./parser ../Test/pretest/17.txt;
	./parser ../Test/pretest/18.txt;
	./parser ../Test/pretest/19.txt;
	./parser ../Test/pretest/20.txt;
	./parser ../Test/pretest/21.txt;
	./parser ../Test/pretest/22.txt;
	./parser ../Test/pretest/23.txt;
	./parser ../Test/lab2_more/A-1.cmm;
	./parser ../Test/lab2_more/A-2.cmm;
	./parser ../Test/lab2_more/A-3.cmm;
	./parser ../Test/lab2_more/A-4.cmm;
	./parser ../Test/lab2_more/A-5.cmm;
	./parser ../Test/lab2_more/A-6.cmm;
	./parser ../Test/lab2_more/A-7.cmm;
	./parser ../Test/lab2_more/A-12.cmm;
	./parser ../Test/lab2_more/A-8.cmm;
	./parser ../Test/lab2_more/A-9.cmm;
	./parser ../Test/lab2_more/A-15.cmm;

clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h)
	rm -f *~
