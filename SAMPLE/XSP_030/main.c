/*
	XSP ���p�T���v���v���O����

	[����]
		�R�}���h���C�������Ƃ��Ďw�肳�ꂽ�����X�v���C�g�t�@�C����ǂݍ��݁A
		��ʂɕ\�����܂��B�W���C�X�e�B�b�N�ŕ����X�v���C�g���ړ��A�g���K��
		�p���b�g�ƃp�^�[����ύX�ł��܂��B�g���K�������ő�����\�ł��B��
		�ՓI�ȕ����X�v���C�g�r���[���Ƃ��Ă����p�ł��܂��B

	[���]
		�����X�v���C�g�`��f�[�^�i�g���q .xsp .frm .ref�j���t�@�C������ǂ�
		���ݕ\������菇�������T���v���v���O�����ł��B

		ZAKO.xsp/frm/ref ���A�{�v���O�����Ή��̃T���v���f�[�^�ł��BZAKO.SP
		�� PCG �f�[�^�AZAKO.src �͕����X�v���C�g�`��f�[�^�̃\�[�X�t�@�C��
		�ł��BZAKO.xsp/frm/ref �́ACVOBJ.X ���g�p���� ZAKO.src �� ZAKO.SP
		���琶�����ꂽ�t�@�C���ł��B

		�{�v���O�����́A�X�v���C�g�p���b�g�̏������͍s���܂���B�X�v���C�g
		RAM��Ɍ��ݐݒ肳��Ă���p���b�g�����̂܂ܕ\�����ʂɔ��f����܂��B
		�K�v�ȏꍇ�͎��O�ɑ��̃c�[����p����Ȃǂ��ăp���b�g���������s����
		�������B
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <doslib.h>
#include <iocslib.h>
#include "../../XSP/XSP2lib.H"

/* �X�v���C�g PCG �p�^�[���ő�g�p�� */
#define		PCG_MAX		2048

/* �����X�v���C�g���t�@�����X�f�[�^�ő�g�p�� */
#define		REF_MAX		4096

/* �����X�v���C�g�t���[���f�[�^�ő�g�p�� */
#define		FRM_MAX		16384


/*
	XSP �p PCG �z�u�Ǘ��e�[�u��
	�X�v���C�g PCG �p�^�[���ő�g�p�� + 1 �o�C�g�̃T�C�Y���K�v�B
*/
char	pcg_alt[PCG_MAX + 1];

/* PCG �f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
char	pcg_dat[PCG_MAX * 128];

/* XSP �����X�v���C�g�t���[���f�[�^ */
XOBJ_FRM_DAT	frm_dat[FRM_MAX];

/* XSP �����X�v���C�g���t�@�����X�f�[�^ */
XOBJ_REF_DAT	ref_dat[REF_MAX];


/*-------------------------------------[ MAIN ]---------------------------------------*/
void main(int argc, unsigned char* argv[])
{
	int		i;
	int		sizeof_ref;		/* �����X�v���C�g���t�@�����X�f�[�^�ǂݍ��ݐ� */
	FILE	*fp;

	/* �L�����N�^�Ǘ��\���� */
	struct {
		short	x, y;		/* ���W */
		short	pt;			/* �X�v���C�g�p�^�[�� No. */
		short	info;		/* ���]�R�[�h�E�F�E�D��x��\���f�[�^ */
	} player;


	/*----------[ �R�}���h���C����́`�t�@�C���ǂݍ��� ]----------*/

	if (argc <= 1) {
		/* �w���v��\�����ďI�� */
		printf("�g�p�@�Fmain.X [�`��f�[�^�t�@�C�����i�g���q�ȗ��j]\n");
		exit(0);
	} else {
		char	str_tmp[256];

		/* �t�@�C���ǂݍ��� */
		strmfe(str_tmp, argv[1], "xsp");		/* �g���q�u�� */
		fp = fopen(str_tmp, "rb");
		if (fp == NULL) {
			CRTMOD(0x10);
			printf("%s �� open �ł��܂���B\n", str_tmp);
			exit(1);
		}
		fread(pcg_dat, sizeof(char), 128 * PCG_MAX, fp);

		strmfe(str_tmp, argv[1], "frm");		/* �g���q�u�� */
		fp = fopen(str_tmp, "rb");
		if (fp == NULL) {
			CRTMOD(0x10);
			printf("%s �� open �ł��܂���B\n", str_tmp);
			exit(1);
		}
		fread(frm_dat, sizeof(XOBJ_FRM_DAT), FRM_MAX, fp);

		strmfe(str_tmp, argv[1], "ref");		/* �g���q�u�� */
		fp = fopen(str_tmp, "rb");
		if (fp == NULL) {
			CRTMOD(0x10);
			printf("%s �� open �ł��܂���B\n", str_tmp);
			exit(1);
		}
		sizeof_ref = fread(ref_dat, sizeof(XOBJ_REF_DAT), REF_MAX, fp);

		fcloseall();

		/* REF_DAT[].ptr �␳ */
		for (i = 0; i < sizeof_ref; i++) {
			ref_dat[i].ptr = (void *)((intptr_t)ref_dat[i].ptr + (intptr_t)(&frm_dat[0]));
		}
	}


	/*---------------------[ ��ʂ������� ]---------------------*/

	/* 256x256 dot 16 �F�O���t�B�b�N�v���[�� 4 �� 31KHz */
	CRTMOD(6);

	/* �X�v���C�g�\���� ON */
	SP_ON();

	/* BG0 �\�� OFF */
	BGCTRLST(0, 0, 0);

	/* BG1 �\�� OFF */
	BGCTRLST(1, 1, 0);

	/* �ȈՐ��� */
	printf(
		"�W���C�X�e�B�b�N�ŕ����X�v���C�g���ړ��ł��܂��B\n"
		"�g���K�Ńp�^�[���ƃp���b�g��ύX�ł��܂��B\n"
		"�����L�[�������ƏI�����܂��B\n"
	);

	/* �J�[�\���\�� OFF */
	B_CUROFF();


	/*---------------------[ XSP �������� ]---------------------*/

	/* XSP �̏����� */
	xsp_on();

	/* PCG �f�[�^�� PCG �z�u�Ǘ����e�[�u�����w�� */
	xsp_pcgdat_set(pcg_dat, pcg_alt, sizeof(pcg_alt));

	/* �����X�v���C�g�`��f�[�^���w�� */
	xsp_objdat_set(ref_dat);


	/*==========================[ �X�e�B�b�N�ő��삷��f�� ]============================*/

	/* ������ */
	player.x	= 0x88;		/* X ���W�����l */
	player.y	= 0x88;		/* Y ���W�����l */
	player.pt	= 0;		/* �X�v���C�g�p�^�[�� No. */
	player.info	= 0x013F;	/* ���]�R�[�h�E�F�E�D��x��\���f�[�^ */


	/* �����L�[�������܂Ń��[�v */
	while (INPOUT(0xFF) == 0) {
		static int	pre_stk = 0;
		static int	stk = 0;
		static int	timer = 0;

		/* �������� */
		xsp_vsync2(1);

		/* �X�e�B�b�N�̓��͂ɍ����Ĉړ� */
		pre_stk = stk;		/* �O��̃X�e�B�b�N�̓��e */
		stk = JOYGET(0);	/* ����̃X�e�B�b�N�̓��e */
		if ((stk & 1) == 0) player.y -= 1;		/* ��Ɉړ� */
		if ((stk & 2) == 0) player.y += 1;		/* ���Ɉړ� */
		if ((stk & 4) == 0) player.x -= 1;		/* ���Ɉړ� */
		if ((stk & 8) == 0) player.x += 1;		/* �E�Ɉړ� */

		/* �g���K���������ԑ��� */
		if ((stk & 0x60) == (pre_stk & 0x60)) {
			timer++;		/* �^�C�}���Z */
		} else {
			timer = 0;		/* �^�C�}�N���A */
		}

		/* �g���K�[ 2 �������ꂽ��p�^�[���ύX */
		if ((stk & 0x20) == 0  &&  ((pre_stk & 0x20) != 0  ||  timer > 32)) {
			player.pt++;
			if (player.pt >= sizeof_ref) player.pt = 0;
		}

		/* �g���K�[ 1 �������ꂽ��F�ύX */
		if ((stk & 0x40) == 0  &&  ((pre_stk & 0x40) != 0  ||  timer > 32)) {
			if ((player.info & 0x0F00) == 0x0F00) {
				player.info &= 0xF0FF;		/* �J���[�R�[�h�� 0 �ɖ߂� */
			} else {
				player.info += 0x100;		/* �J���[�R�[�h�� 1 ���Z */
			}
		}

		/* pt info ����ʂɕ\�� */
		B_LOCATE(0, 5);
		printf("  pt = %3X \n", player.pt);
		printf("info = %3X \n", player.info);

		/* �X�v���C�g�̕\���o�^ */
		xobj_set(player.x, player.y, player.pt, player.info);
		/*
			�������́A
			xobj_set_st(&player);
			�ƋL�q����΁A��荂���Ɏ��s�ł���B
		*/

		/* �X�v���C�g���ꊇ�\������ */
		xsp_out();
	}


	/*-----------------------[ �I������ ]-----------------------*/

	/* XSP �̏I������ */
	xsp_off();

	/* ��ʃ��[�h��߂� */
	CRTMOD(0x10);
}


