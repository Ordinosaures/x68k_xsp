/*
	XSP ���p�T���v���v���O����

	[����]
		XSP ���c��ʃ��[�h�ɐݒ肵�A�X�v���C�g�� 1 ���\�����܂��B�X�v���C�g
		���W���C�X�e�B�b�N�� 8 �����Ɉړ��ł��܂��B

	[���]
		XSP �̏c��ʃ��[�h�̗��p�菇�������T���v���v���O�����ł��B�c��ʃ��[
		�h�ł́A���j�^�̍�������ʉ����ƂȂ�܂��B
*/

#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <iocslib.h>
#include "../../XSP/XSP2lib.H"
#include "../../PCG90/PCG90.H"

/* �X�v���C�g PCG �p�^�[���ő�g�p�� */
#define	PCG_MAX		256


/*
	XSP �p PCG �z�u�Ǘ��e�[�u��
	�X�v���C�g PCG �p�^�[���ő�g�p�� + 1 �o�C�g�̃T�C�Y���K�v�B
*/
char	pcg_alt[PCG_MAX + 1];

/* PCG �f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
char	pcg_dat[PCG_MAX * 128];
  
/* �p���b�g�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
unsigned short pal_dat[256];


/*-------------------------------------[ MAIN ]---------------------------------------*/
void main()
{
	int		i;
	FILE	*fp;

	/* �L�����N�^�Ǘ��\���� */
	struct {
		short	x, y;		/* ���W */
		short	pt;			/* �X�v���C�g�p�^�[�� No. */
		short	info;		/* ���]�R�[�h�E�F�E�D��x��\���f�[�^ */
	} player;


	/*---------------------[ ��ʂ������� ]---------------------*/

	/* 256x256dot 16 �F�O���t�B�b�N�v���[�� 4 �� 31KHz */
	CRTMOD(6);

	/* �X�v���C�g�\���� ON */
	SP_ON();

	/* BG0 �\�� OFF */
	BGCTRLST(0, 0, 0);

	/* BG1 �\�� OFF */
	BGCTRLST(1, 1, 0);

	/* �ȈՐ��� */
	printf(
		"�W���C�X�e�B�b�N�ŃX�v���C�g���ړ��ł��܂��B\n"
		"�����L�[�������ƏI�����܂��B\n"
	);

	/* �J�[�\���\�� OFF */
	B_CUROFF();


	/*------------------[ PCG �f�[�^�ǂݍ��� ]------------------*/

	fp = fopen("../PANEL.SP", "rb");
	if (fp == NULL) {
		CRTMOD(0x10);
		printf("../PANEL.SP �� open �ł��܂���B\n");
		exit(1);
	}
	fread(
		pcg_dat,
		128,		/* 1PCG = 128byte */
		256,		/* 256PCG */
		fp
	);
	fclose(fp);

	/*-----------[ PCG �f�[�^���c��ʗp�� 90 �x��] ]-----------*/

	for (i = 0; i < 256; i++) {
		pcg_roll90(&pcg_dat[i * 128], 1);
	}

	/*--------[ �X�v���C�g�p���b�g�f�[�^�ǂݍ��݂ƒ�` ]--------*/

	fp = fopen("../PANEL.PAL", "rb");
	if (fp == NULL) {
		CRTMOD(0x10);
		printf("../PANEL.PAL �� open �ł��܂���B\n");
		exit(1);
	}
	fread(
		pal_dat,
		2,			/* 1color = 2byte */
		256,		/* 16color * 16block */
		fp
	);
	fclose(fp);

	/* �X�v���C�g�p���b�g�ɓ]�� */
	for (i = 0; i < 256; i++) {
		SPALET((i & 15) | (1 << 0x1F), i / 16, pal_dat[i]);
	}


	/*---------------------[ XSP �������� ]---------------------*/

	/* XSP �̏����� */
	xsp_on();

	/* PCG �f�[�^�� PCG �z�u�Ǘ����e�[�u�����w�� */
	xsp_pcgdat_set(pcg_dat, pcg_alt, sizeof(pcg_alt));

	/* �c��ʃ��[�h */
	xsp_vertical(1);


	/*===========================[ �X�e�B�b�N�ő��삷��f�� ]=============================*/

	/* ������ */
	player.x	= 0x88;		/* X ���W�����l */
	player.y	= 0x88;		/* Y ���W�����l */
	player.pt	= 0;		/* �X�v���C�g�p�^�[�� No. */
	player.info	= 0x013F;	/* ���]�R�[�h�E�F�E�D��x��\���f�[�^ */

	/* �����L�[�������܂Ń��[�v */
	while (INPOUT(0xFF) == 0) {
		int	stk;

		/* �������� */
		xsp_vsync2(1);

		/* �X�e�B�b�N�̓��͂ɍ����Ĉړ� */
		stk = JOYGET(0);
		if ((stk & 1) == 0  &&  player.y > 0x010) player.y -= 1;	/* ��Ɉړ� */
		if ((stk & 2) == 0  &&  player.y < 0x100) player.y += 1;	/* ���Ɉړ� */
		if ((stk & 4) == 0  &&  player.x > 0x010) player.x -= 1;	/* ���Ɉړ� */
		if ((stk & 8) == 0  &&  player.x < 0x100) player.x += 1;	/* �E�Ɉړ� */

		/* �X�v���C�g�̕\���o�^ */
		xsp_set(player.x, player.y, player.pt, player.info);
		/*
			�������́A
				xsp_set_st(&player);
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



