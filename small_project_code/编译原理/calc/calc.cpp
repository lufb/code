/**
 *	simple integer arithmetic calculator
 *	according to the EBNF:
 *
 *	<program> -> {exp;} 
 *	<exp> -> <term> { <addop> <term> }
 *	<addop> -> + | -
 *	<term> -> <factor> { <mulop> <factor> }
 *	<mulop> -> * | /
 *	<factor> -> ( <exp> ) | number | -number
 *
 *	Inputs a line of text from stdin
 *	Outputs "Error" or the result.
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "syntax_tree.h"



char	token;

street_node* exp(void);
street_node* term(void);
street_node* factor(void);

void
error(void)
{
	fprintf(stderr, "Error\n"); 
	exit(0);
}

void
match(char excpd_token)
{
	if(token == excpd_token)
		token = getchar();
 	else
 		error();
}

/*
	<program> -> {exp;} 
*/
street_node*
program()
{
	street_node *head = NULL;
	street_node *temp;
/* 以下形式是以栈形式返回 */
	while(token != '\n'){
		temp = exp();
		if(temp)	/* 这里判断是为了让一个语句出错后，其它语句也可形成语法树 */
		{
			if(head == NULL)
				head = temp;
			else
			{
				temp->next = head;
				head = temp;
			}
		}
		match(';');
	}
	
	return head;
}

/*
	<exp> -> <term> { <addop> <term> }
*/
street_node*
exp(void)
{
	street_node			*temp = term();
	street_node			*new_node;

	while((token == '+') || (token == '-'))
		switch(token){
		case '+': 
			match('+');
			//temp += term();
			new_node = alloc_node();
			new_node->val = '+';
			new_node->lchild = temp;
			new_node->rchild = term();
			temp = new_node;
			break;
		case '-':
			match('-');
			//temp -= term();
			new_node = alloc_node();
			new_node->val = '-';
			new_node->lchild = temp;
			new_node->rchild = term();
			temp = new_node;
			break;
	}
	return temp;
}

/*
	<term> -> <factor> { <mulop> <factor> }
*/
street_node*
term(void)
{
	street_node		*temp = factor();
	street_node		*new_node;

	while((token == '*') || (token == '/')){
		switch(token){
		case '*':
			match('*');
			//temp *= factor();
			new_node = alloc_node();
			new_node->val = '*';
			new_node->lchild = temp;
			new_node->rchild = factor();
			temp = new_node;
			break;
		case '/':
			match('/');
			//temp /= factor();
			new_node = alloc_node();
			new_node->val = '/';
			new_node->lchild = temp;
			new_node->rchild = factor();
			temp = new_node;
			break;
		}
	}

	return temp;
}

/*
	<factor> -> ( <exp> ) | number
*/
street_node*
factor(void)
{
	street_node		*temp = NULL, *new_node;

	if(token == '('){
		match('(');
		temp = exp();
		match(')');	
	}
	else if(token == '-')
	{
		int			num;
		match('-');
		ungetc(token, stdin);
		scanf("%d", &num);
		token = getchar();
		new_node = alloc_node();
		new_node->val = -num;
		temp = new_node;
	}
	else if(isdigit(token)){
		int			num;
		ungetc(token, stdin);
		scanf("%c", &num);
		token = getchar();
		
		new_node = alloc_node();
		new_node->val = num;

		temp = new_node;
	}else{
		error();
	}

	return temp;
}


void
parase()
{
	street_node		*result;
	
	token = getchar();
	result = program();
	pr_tree(result);
	printf("\n");
}

int
main()
{	
	while(1)
		parase();

	return 0;
}


