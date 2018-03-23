/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   x86.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/23 09:58:17 by pfichepo          #+#    #+#             */
/*   Updated: 2018/03/23 09:42:22 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <nm.h>

static void	swaparray(struct nlist *array, bool swap)
{
	if (!swap)
		return ;
	array->n_value = swap_uint32(array->n_value);
	array->n_desc = swap_uint16(array->n_desc);
	array->n_un.n_strx = swap_uint32(array->n_un.n_strx);
}

static void	swapsym(struct symtab_command *sym, bool swap)
{
	if (!swap)
		return ;
	sym->nsyms = swap_uint32(sym->nsyms);
	sym->symoff = swap_uint32(sym->symoff);
	sym->stroff = swap_uint32(sym->stroff);
}

static void	add_output(int nsyms, void *symoff,
	void *stroff, t_env *env)
{
	int				i;
	char			*stringtable;
	struct nlist	*array;
	t_cmd			*cmd;

	array = (void*)symoff;
	stringtable = (void*)stroff;
	cmd = NULL;
	i = 0;
	while (i < nsyms)
	{
		swaparray(&array[i], env->tmp_swap);
		segfaultcheck((char*)(&array[i]), env->end, AT);
		segfaultcheck(stringtable, env->end, AT);
		mlccmd(env, array[i].n_value, typing(array[i].n_type, array[i].n_sect,
			env->section, array[i].n_value),
		stringtable + array[i].n_un.n_strx);
		i++;
	}
}

static void	browse_lc(int ncmds, bool swap, t_env *env,
	struct mach_header *header)
{
	int					i;
	struct load_command	*lc;

	lc = (struct load_command*)(header + 1);
	i = 0;
	env->tmp_swap = swap;
	while (i++ < ncmds)
	{
		segfaultcheck((char*)lc, env->end, AT);
		lc->cmdsize = (swap) ? swap_uint32(lc->cmdsize) : lc->cmdsize;
		lc->cmd = (swap) ? swap_uint32(lc->cmd) : lc->cmd;
		if (lc->cmd == LC_SEGMENT)
			add_segment32((struct segment_command*)lc, env->section, swap);
		lc = (void*)lc + lc->cmdsize;
	}
}

void		handle_32(t_env *env, char *adr, char *max, bool swap)
{
	int						ncmds;
	struct mach_header		*header;
	struct load_command		*lc;
	int						i;
	struct symtab_command	*sym;

	header = (struct mach_header*)adr;
	segfaultcheck((char*)header, max, AT);
	ncmds = (swap) ? swap_uint32(header->ncmds) : header->ncmds;
	browse_lc(ncmds, swap, env, header);
	lc = (struct load_command*)(header + 1);
	i = 0;
	while (i++ < ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			sym = (struct symtab_command *)lc;
			swapsym(sym, swap);
			add_output(sym->nsyms, (void*)adr + sym->symoff,
				(void*)adr + sym->stroff, env);
		}
		lc = (void*)lc + lc->cmdsize;
	}
	order_cmds(env);
	print_cmds(env->list, 8);
}