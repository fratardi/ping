
NAME = ping

MAIN = ping.c

FLAGS = #-Wall -Wextra -Werror

FLAG__DBG = -fsanitize=address

SRCDIR = #./src/

COMPILER = clang

RM = /bin/rm -rf

FUNCTIONS = print1.c#addtochain.c buffconvert.c parse_file.c \
			#ft_display.c ft_intarrop.c ft_solve.c

SRCO = $(addprefix $(SRCDIR), $(FUNCTIONS:.c=.o)) \
		$(MAIN:.c=.o)

HEADERS = ./ping.h

%.o: %.c $(HEADERS)
	$(COMPILER) $(FLAGS) -o $@ -c $<

all: $(NAME)

$(NAME): $(SRCO)
#	make -C libft
	$(COMPILER) $(FLAGS) -o $(NAME) $(SRCO)


dbg: $(NAME)
	$(COMPILER) $(FLAG__DBG) -o $(NAME) $(SRCO)


clean:
	make 
	$(RM) $(SRCO)

fclean: clean
	make 
	$(RM) $(NAME)

re: fclean all