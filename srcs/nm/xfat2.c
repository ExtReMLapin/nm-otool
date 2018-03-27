/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xfat2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/20 09:36:17 by pfichepo          #+#    #+#             */
/*   Updated: 2018/03/27 11:27:35 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <nm.h>

char		*get_cputype(cpu_type_t cputype)
{
	if (cputype == CPU_TYPE_VAX)
		return ("vax");
	else if (cputype == CPU_TYPE_MC680x0)
		return ("mc680");
	else if (cputype == CPU_TYPE_X86 || cputype == CPU_TYPE_I386)
		return ("i386");
	else if (cputype == CPU_TYPE_X86_64)
		return ("x86_64");
	else if (cputype == CPU_TYPE_MC98000)
		return ("mc98000");
	else if (cputype == CPU_TYPE_HPPA)
		return ("hppa");
	else if (cputype == CPU_TYPE_ARM)
		return ("arm");
	else if (cputype == CPU_TYPE_SPARC)
		return ("sparc");
	else if (cputype == CPU_TYPE_I860)
		return ("i860");
	else if (cputype == CPU_TYPE_POWERPC)
		return ("ppc");
	else if (cputype == CPU_TYPE_POWERPC64)
		return ("ppc64");
	else
		return ("?");
}

static struct fat_arch *firstfoundcpu(struct fat_arch *a, cpu_type_t cpu, uint32_t n)
{
	struct fat_arch *ret;

	ret = NULL;
	while (n--)
	{
		if (a->cputype == cpu || (cpu_type_t)swap_uint32(a->cputype) == cpu)
			return (a);
		a = (void*)a + sizeof(struct fat_arch);
	}
	return (ret);
}

/*
** Could make a get_cputype in bool to get faster result but im lazy
*/

bool		shouldprintcpu(struct fat_arch *c, struct fat_arch *arch, uint32_t n)
{
	if (get_cputype(c->cputype)[0] == '?')
		return (false);
	if (firstfoundcpu(arch, c->cputype, n) != c)
		return (false);
	if (c->cputype == CPU_TYPE_X86 || c->cputype == CPU_TYPE_I386)
		return (firstfoundcpu(arch, CPU_TYPE_X86_64, n) == NULL);
	return (true);
}

uint32_t	how_many_cpu(struct fat_arch *a, uint32_t n)
{
	bool			has_i386;
	bool			has_x86_x64;
	unsigned int	count;
	cpu_type_t		c;

	has_i386 = false;
	has_x86_x64 = false;
	count = 0;
	while (n--)
	{
		c = a->cputype;
		if (c == CPU_TYPE_X86 || c == CPU_TYPE_I386 ||
			swap_uint32(c) == CPU_TYPE_X86 || swap_uint32(c) == CPU_TYPE_I386)
		{
			has_i386 = true;
		}
		if (c == CPU_TYPE_X86_64 || swap_uint32(c) == CPU_TYPE_X86_64)
			has_x86_x64 = true;
		if ((get_cputype(c)[0] != '?' || get_cputype(swap_uint32(c))[0] != '?') && firstfoundcpu(a, c, n) == a)
			count++;
		a = (void*)a + sizeof(struct fat_arch);
	}
	if (has_x86_x64 && has_i386)
		count--;
	return (count);
}
