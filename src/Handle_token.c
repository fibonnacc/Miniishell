/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_token.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:26:40 by helfatih          #+#    #+#             */
/*   Updated: 2025/05/16 17:26:43 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

t_token *creat_token(char *line, t_token_type type)
{
	t_token *new_token;

	if (!(new_token = (t_token *)ft_calloc(1, sizeof(t_token))))
		return (NULL);
	if (!(new_token->av = ft_strdup(line)))
		return (free(new_token), NULL);
	new_token->type = type;
	new_token->should_not_expand = false;
	new_token->should_expand = false;
	new_token->next = NULL;
	return (new_token);
}

void	handle_quote(bool	*in_quot ,char *quot_char, int *i, char *line)
{
	if ((line[*i] == '"' || line[*i] == '\''))
	{
		if (*quot_char == '\0')
		{
			*in_quot = true;
			*quot_char = line[*i];
		}
		else if (*quot_char == line[*i])
		{
			*in_quot = false;
			*quot_char = 0;
		}
	}
}

void	add_token(t_token **token, t_token *new_token)
{
	t_token *current;

	if (!*token)
	{
		*token = new_token;
		return ;
	}
	current = *token;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

void	init_variables(char *str, int *i, int *j, bool *in_quotes, char *quote_char, size_t *len, char **result)
{
	*in_quotes = false;
	*j = 0;
	*i = 0;
	*quote_char = 0;
	*len = ft_strlen(str);
	*result = malloc(*len + 1);
}

char *remove_quotes(char *str)
{
    if (!str)
		return NULL;

    int i = 0, j = 0;
    bool in_single = false;
    bool in_double = false;
    char *result = malloc(strlen(str) + 1);
    if (!result) return NULL;

    while (str[i])
	{
        if (str[i] == '\'' && !in_double)
		{
            in_single = !in_single;
            i++;
            continue;
        }
        else if (str[i] == '"' && !in_single)
		{
            in_double = !in_double;
            i++;
            continue;
        }
        if (in_single)
		{
            result[j++] = str[i++];
        }
        else
		{
            result[j++] = str[i++];
        }
    }
    result[j] = '\0';
    return result;
}

bool is_closed_quotes(char *str)
{
	int	i;
    bool in_single;
    bool in_double;

	i = 0;
	in_double = false;
	in_single = false;
    while (str[i])
    {
        if (str[i] == '\'' && !in_double)
            in_single = !in_single;
        else if (str[i] == '"' && !in_single)
            in_double = !in_double;
		i++;
    }
    return (!in_single && !in_double);
}

bool	special_character(char *str)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '\\' || str[i] == ';')
			return (false);
		i++;
	}
	return (true);
}

void	handle_word_token(t_token **token, int start, char *line, int *i, bool should_expand, bool should_not_expand)
{
	char *word;

	if (*i > start)
	{
		word = ft_substr(line, start, *i - start);
		if (!word)
			return;
		if (!special_character(word))
		{
			printf("this character is not valid\n");
			return;
		}
		if (!is_closed_quotes(word))
		{
			printf ("the quote does not close!\n");
			free(word);
			return;
		}
		if (word && *word != '\0')
		{
			t_token *new = creat_token(line, get_token_type(line));
			if (new)
			{
				new->should_not_expand = should_not_expand;
				new->should_expand = should_expand;
			}
			char *str = expand_env(word, &new);
			if (str)
			{
				free(new->av);
				new->av = str;
			}
			add_token(token, new);
			free(word);
		}
	}
}

int	handle_speciale_token(t_token **token, char *line, int i)
{
	char	special[3];

	if ((line[i] == '>' || line[i] == '<') && (line[i + 1] == line[i]))
	{
		special[0] = line[i];
		special[1] = line[i];
		special[2] = '\0';
		add_token(token, creat_token(special, get_token_type(special)));
		return (i + 2);
	}
	else
	{
		special[0] = line[i];
		special[1] = '\0';
		add_token(token, creat_token(special, get_token_type(special)));
		return (i + 1);
	}
}
