/*  Copyright 1997-2003,2006,2007,2009 Alain Knaff.
 *  This file is part of mtools.
 *
 *  Mtools is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Mtools is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Mtools.  If not, see <http://www.gnu.org/licenses/>.
 *
 * mlabel.c
 * Make an MSDOS volume label
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mainloop.h"
#include "vfat.h"
#include "mtools.h"
#include "nameclash.h"

static void usage(int ret) NORETURN;
static void usage(int ret)
{
	fprintf(stderr, 
		"Mtools version %s, dated %s\n", mversion, mdate);
	fprintf(stderr, 
		"Usage: %s [-v] drive\n", progname);
	exit(ret);
}


static void displayInfosector(Stream_t *Stream, union bootsector *boot)
{
	InfoSector_t *infosec;

	if(WORD(ext.fat32.infoSector) == MAX16)
		return;

	infosec = (InfoSector_t *) safe_malloc(WORD(secsiz));
	force_read(Stream, (char *) infosec, 
			   (mt_off_t) WORD(secsiz) * WORD(ext.fat32.infoSector),
			   WORD(secsiz));
	printf("\nInfosector:\n");
	printf("signature=0x%08x\n", _DWORD(infosec->signature1));
	if(_DWORD(infosec->count) != MAX32)
		printf("free clusters=%u\n", _DWORD(infosec->count));
	if(_DWORD(infosec->pos) != MAX32)
		printf("last allocated cluster=%u\n", _DWORD(infosec->pos));
}


void minfo(int argc, char **argv, int type)
{
	union bootsector boot;

	char name[EXPAND_BUF];
	int media;
	unsigned long tot_sectors;
	int size_code;
	int sector_size;
	int i;
	struct device dev;
	char drive;
	int verbose=0;
	int c;
	Stream_t *Stream;
	struct label_blk_t *labelBlock;

	unsigned long sect_per_track;
	int tracks_match=0;
	int hidden;
	
	if(helpFlag(argc, argv))
		usage(0);
	while ((c = getopt(argc, argv, "i:vh")) != EOF) {
		switch (c) {
			case 'i':
				set_cmd_line_image(optarg, 0);
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
				usage(0);
			default:
				usage(1);
		}
	}

	if(argc == optind)
		usage(1);

	for(;optind < argc; optind++) {
		if(!argv[optind][0] || argv[optind][1] != ':')
			usage(1);
		drive = toupper(argv[optind][0]);

		if(! (Stream = find_device(drive, O_RDONLY, &dev, &boot, 
					   name, &media, 0, NULL)))
			exit(1);

		tot_sectors = DWORD_S(bigsect);
		SET_INT(tot_sectors, WORD_S(psect));
		sector_size = WORD_S(secsiz);
		size_code=2;
		for(i=0; i<7; i++) {
			if(sector_size == 128 << i) {
				size_code = i;
				break;
			}
		}
		printf("device information:\n");
		printf("===================\n");
		printf("filename=\"%s\"\n", name);
		printf("sectors per track: %d\n", dev.sectors);
		printf("heads: %d\n", dev.heads);
		printf("cylinders: %d\n\n", dev.tracks);

		sect_per_track = dev.sectors * dev.heads;
		if(sect_per_track != 0) {
			printf("mformat command line: mformat ");
			hidden = DWORD_S(nhs);
			if(tot_sectors ==
			   dev.tracks * sect_per_track - hidden % sect_per_track) {
				tracks_match=1;
				printf("-t %d", dev.tracks);
			} else {
				printf("-T %ld", tot_sectors);
			}
			printf (" -h %d -s %d ", dev.heads, dev.sectors);
			if(hidden || !tracks_match)
				printf("-H %d ", hidden);
			if(size_code != 2)
				printf("-S %d ",size_code);
			printf("%c:\n", tolower(drive));
			printf("\n");
		}
		printf("bootsector information\n");
		printf("======================\n");
		printf("banner:\"%8s\"\n", boot.boot.banner);
		printf("sector size: %d bytes\n", WORD_S(secsiz));
		printf("cluster size: %d sectors\n", boot.boot.clsiz);
		printf("reserved (boot) sectors: %d\n", WORD_S(nrsvsect));
		printf("fats: %d\n", boot.boot.nfat);
		printf("max available root directory slots: %d\n", 
		       WORD_S(dirents));
		printf("small size: %d sectors\n", WORD_S(psect));
		printf("media descriptor byte: 0x%x\n", boot.boot.descr);
		printf("sectors per fat: %d\n", WORD_S(fatlen));
		printf("sectors per track: %d\n", WORD_S(nsect));
		printf("heads: %d\n", WORD_S(nheads));
		printf("hidden sectors: %d\n", DWORD_S(nhs));
		printf("big size: %d sectors\n", DWORD_S(bigsect));

		if(WORD_S(fatlen)) {
		    labelBlock = &boot.boot.ext.old.labelBlock;
		} else {
		    labelBlock = &boot.boot.ext.fat32.labelBlock;
		}

		printf("physical drive id: 0x%x\n", 
		       labelBlock->physdrive);
		printf("reserved=0x%x\n", 
		       labelBlock->reserved);
		printf("dos4=0x%x\n", 
		       labelBlock->dos4);
		printf("serial number: %08X\n", 
		       _DWORD(labelBlock->serial));
		printf("disk label=\"%11.11s\"\n", 
		       labelBlock->label);
		printf("disk type=\"%8.8s\"\n", 
		       labelBlock->fat_type);

		if(!WORD_S(fatlen)){
			printf("Big fatlen=%u\n",
			       DWORD_S(ext.fat32.bigFat));
			printf("Extended flags=0x%04x\n",
			       WORD_S(ext.fat32.extFlags));
			printf("FS version=0x%04x\n",
			       WORD_S(ext.fat32.fsVersion));
			printf("rootCluster=%u\n",
			       DWORD_S(ext.fat32.rootCluster));
			if(WORD_S(ext.fat32.infoSector) != MAX16)
				printf("infoSector location=%d\n",
				       WORD_S(ext.fat32.infoSector));
			if(WORD_S(ext.fat32.backupBoot) != MAX16)
				printf("backup boot sector=%d\n",
				       WORD_S(ext.fat32.backupBoot));
			displayInfosector(Stream,&boot);
		}

		if(verbose) {
			int size;
			unsigned char *buf;

			printf("\n");
			size = WORD_S(secsiz);
			
			buf = (unsigned char *) malloc(size);
			if(!buf) {
				fprintf(stderr, "Out of memory error\n");
				exit(1);
			}

			size = READS(Stream, buf, (mt_off_t) 0, size);
			if(size < 0) {
				perror("read boot sector");
				exit(1);
			}

			print_sector("Boot sector hexdump", buf, size);
		}
	}
	FREE(&Stream);
	exit(0);
}
