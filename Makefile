
NAME = ping

MAIN = ping.c

FLAGS = -pthread

FLAG__DBG =   -g3  -Wuninitialized -Wall -Wextra -Werror 

SRCDIR = #./src/

COMPILER = clang

RM = /bin/rm -rf

FUNCTIONS = print1.c   pinger.c  init_ping.c 

SRCO = $(addprefix $(SRCDIR), $(FUNCTIONS:.c=.o)) \
		$(MAIN:.c=.o)

HEADERS = ./ping.h

%.o: %.c $(HEADERS)
	$(COMPILER) $(FLAGS) -o $@ -c $<

all: $(NAME)

$(NAME): $(SRCO)
#	make -C libft
	$(COMPILER) $(FLAGS) $(FLAG__DBG) -o $(NAME) $(SRCO)


dbg: $(NAME)
	$(COMPILER) $(FLAG__DBG) -o $(NAME) $(SRCO)


clean:
	make 
	$(RM) $(SRCO)

fclean: clean
	make 
	$(RM) $(NAME)

re: fclean all