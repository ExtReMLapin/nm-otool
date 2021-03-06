/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xfat.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anonymous <anonymous@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/23 09:58:23 by pfichepo          #+#    #+#             */
/*   Updated: 2018/04/12 16:54:24 by anonymous        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <nm.h>

static void		swapvars32(struct fat_arch *arch, bool swap)
{
	if (!swap)
		return ;
	arch->cputype = swap_uint32(arch->cputype);
	arch->cpusubtype = swap_uint32(arch->cpusubtype);
	arch->offset = swap_uint32(arch->offset);
	arch->size = swap_uint32(arch->size);
	arch->align = swap_uint32(arch->align);
}

static void		swapvars64(struct fat_arch_64 *arch, bool swap)
{
	if (!swap)
		return ;
	arch->cputype = swap_uint64(arch->cputype);
	arch->cpusubtype = swap_uint64(arch->cpusubtype);
	arch->offset = swap_uint64(arch->offset);
	arch->size = swap_uint64(arch->size);
	arch->align = swap_uint64(arch->align);
}

void			printarch(char *filename, char *arch)
{
	ft_putchar('\n');
	ft_putstr(filename);
	ft_putstr(" (for architecture ");
	ft_putstr(arch);
	ft_putstr("):\n");
}

void			handle_fat32(t_env *env, bool swap)
{
	struct fat_header	*h;
	struct fat_arch		*arch;
	struct mach_header	*header2;
	unsigned int		cpucount;

	h = (struct fat_header*)env->ptr;
	arch = (struct fat_arch*)(h + 1);
	h->nfat_arch = (swap) ? swap_uint32(h->nfat_arch) : h->nfat_arch;
	cpucount = h->nfat_arch;
	while (!segfaultcheck((char*)(arch + 1), env->end, AT) && h->nfat_arch--)
	{
		swapvars32(arch, swap);
		header2 = (void*)env->ptr + arch->offset;
		if (spcpu(arch, (struct fat_arch*)(h + 1), cpucount))
		{
			if (how_many_cpu((struct fat_arch*)(h + 1), cpucount) > 1)
				printarch(env->file_name, get_cputype(arch->cputype));
			env->in_ppc = arch->cputype == CPU_TYPE_POWERPC ||
			arch->cputype == CPU_TYPE_POWERPC64;
			nm2(env, (char*)header2, (char*)((void*)header2 + arch->size));
		}
		arch = (void*)arch + sizeof(struct fat_arch);
	}
}

void			handle_fat64(t_env *env, bool swap)
{
	struct fat_header		*h;
	struct fat_arch_64		*arch;
	struct mach_header_64	*header2;
	unsigned int			cpucount;

	h = (struct fat_header*)env->ptr;
	arch = (struct fat_arch_64*)(h + 1);
	h->nfat_arch = (swap) ? swap_uint32(h->nfat_arch) : h->nfat_arch;
	cpucount = h->nfat_arch;
	while (!segfaultcheck((char*)(arch + 1), env->end, AT) && h->nfat_arch--)
	{
		swapvars64(arch, swap);
		header2 = (void*)env->ptr + arch->offset;
		if (spcpu64(arch, (struct fat_arch_64*)(h + 1), cpucount))
		{
			if (how_many_cpu64((struct fat_arch_64*)(h + 1), cpucount) > 1)
				printarch(env->file_name, get_cputype(arch->cputype));
			env->in_ppc = arch->cputype == CPU_TYPE_POWERPC ||
			arch->cputype == CPU_TYPE_POWERPC64;
			nm2(env, (char*)header2, (char*)((void*)header2 + arch->size));
		}
		arch = (void*)arch + sizeof(struct fat_arch_64);
	}
}
