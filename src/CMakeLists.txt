add_executable(minishell processus.c main.c parser.c builtin.c)
install(TARGETS minishell DESTINATION bin/)
