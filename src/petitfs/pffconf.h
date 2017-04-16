/*---------------------------------------------------------------------------/
/  Petit FatFs - Configuration file  R0.03 (C)ChaN, 2014
/---------------------------------------------------------------------------*/

#ifndef _PFFCONF
#define _PFFCONF 4004	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define	_USE_READ	1	/* Enable pf_read() function */
#define	_USE_DIR	0	/* Enable pf_opendir() and pf_readdir() function */
#define	_USE_LSEEK	1	/* Enable pf_lseek() function */
#define	_USE_WRITE	1	/* Enable pf_write() function */

#define _FS_FAT12	0	/* Enable FAT12 */
#define _FS_FAT16	1	/* Enable FAT16 */
#define _FS_FAT32	0	/* Enable FAT32 */


#define _USE_LCC	0	/* Allow lower case characters for path name */

#define _WORD_ACCESS	0


#endif /* _PFFCONF */
