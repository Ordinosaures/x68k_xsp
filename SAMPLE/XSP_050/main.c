/*
	XSP ���p�T���v���v���O����

	[����]
		�X�v���C�g�� 1 ���\�����܂��B�X�v���C�g�̈ʒu�ɍ��킹�āA�O���t�B�N
		�X�v���[�����X�N���[�������܂��B

	[���]
		XSP �ɂ��X�v���C�g�\���͓����Ńo�b�t�@�����O����A�x�����ĉ�ʂ�
		���f����܂��B�w�i�̃X�N���[����p���b�g�ύX���s���ꍇ�A�X�v���C�g
		�\���Ɠ���������ɂ́A���̒x�����l������K�v������܂��B

		XSP �͂��̂悤�ȗ��p�P�[�X��z�肵�Axsp_out2 �֐��ɂ��X�v���C�g
		�\�����N�G�X�g���ɁA�����A�����Ԋ��荞�݊֐��ɗ^����������w��ł�
		�܂��B�����Ŏw�肵�������́A�X�v���C�g�\���Ɠ����t���[�����x������
		�̂������A�����Ԋ��荞�݊֐��ɓ`�����܂��B���̃T���v���v���O����
		�ł́A���̎d�g�݂𗘗p���Ĕw�i�̃X�N���[�����W�𐂒��A�����Ԋ��荞
		�݊֐��ɗ^���A�X�v���C�g�\���Ɠ��������X�N���[�����s���Ă��܂��B
*/

#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <iocslib.h>
#include <math.h>
#include "../../XSP/XSP2lib.H"

/* �X�v���C�g PCG �p�^�[���ő�g�p�� */
#define	PCG_MAX		256


/*------------------------------------[ XSP �֘A ]------------------------------------*/

/*
	XSP �p PCG �z�u�Ǘ��e�[�u��
	�X�v���C�g PCG �p�^�[���ő�g�p�� + 1 �o�C�g�̃T�C�Y���K�v�B
*/
char	pcg_alt[PCG_MAX + 1];

/* PCG �f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
char	pcg_dat[PCG_MAX * 128];

/* �p���b�g�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
unsigned short pal_dat[256];


/*----------------------------------[ sin �e�[�u�� ]----------------------------------*/

/* sin �e�[�u�� */
#define NUM_WAVE_ELEMENTS	(512)
short	wave[NUM_WAVE_ELEMENTS];


/*----------------------[ �����A�����Ԋ��荞�݊֐��ɗ^������� ]----------------------*/

typedef struct {
	short scroll_x;
	short scroll_y;
} VSYNC_INT_ARG;

#define NUM_VSYNC_INT_ARGS	(4)
VSYNC_INT_ARG vsync_int_args[NUM_VSYNC_INT_ARGS] = {0};


/*----------------------------[ �����A�����Ԋ��荞�݊֐� ]----------------------------*/

void vsync_int(const VSYNC_INT_ARG *arg)
{
	if (arg != NULL) {
		/* �O���t�B�N�X��� 0 ��ݒ� */
		SCROLL(0, arg->scroll_x, arg->scroll_y);
	}
}

/*-------------------------------------[ MAIN ]---------------------------------------*/
void main()
{
	int		i;
	FILE	*fp;

	/*---------------------[ ��ʂ������� ]---------------------*/

	/* 256x256 dot 16 �F�O���t�B�b�N�v���[�� 4 �� 31KHz */
	CRTMOD(6);

	/* �O���t�B�b�N�\�� ON */
	G_CLR_ON();

	/* �X�v���C�g�\���� ON */
	SP_ON();

	/* BG0 �\�� OFF */
	BGCTRLST(0, 0, 0);

	/* BG1 �\�� OFF */
	BGCTRLST(1, 1, 0);

	/* �O���t�B�b�N�p���b�g 1 �Ԃ�^�����ɂ��� */
	GPALET(1, 0xFFFF);

	/* �ȈՐ��� */
	printf(
		"�����L�[�������ƏI�����܂��B\n"
	);

	/* �J�[�\���\�� OFF */
	B_CUROFF();

	/* �i�q�͗l��`�� */
	WINDOW(0, 0, 511, 511);
	for (i = 0; i < 512; i+=16) {
		struct LINEPTR arg;
		arg.x1 = 0;
		arg.y1 = i;
		arg.x2 = 511;
		arg.y2 = i;
		arg.color = 1;
		arg.linestyle = 0x5555;
		LINE(&arg);
		arg.x1 = i;
		arg.y1 = 0;
		arg.x2 = i;
		arg.y2 = 511;
		arg.color = 1;
		arg.linestyle = 0x5555;
		LINE(&arg);
	}


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

	/* �����A�����Ԋ��荞�݊J�n */
	xsp_vsyncint_on(vsync_int);


	/*-------------------[ sin �e�[�u���쐬 ]-------------------*/

	for (i = 0; i < NUM_WAVE_ELEMENTS; i++) {
		wave[i] = sin(3.1415926535898 * 2 * (double)i / NUM_WAVE_ELEMENTS) * 64;
	}


	/*===========================[ �X�e�B�b�N�ő��삷��f�� ]=============================*/

	{
		/* �t���[���J�E���g */
		short frame_count = 0;

		/* �����L�[�������܂Ń��[�v */
		while (INPOUT(0xFF) == 0) {
			short x, y;

			/* �����A�����Ԋ��荞�݊֐��̈��� */
			VSYNC_INT_ARG *arg = &vsync_int_args[frame_count % NUM_VSYNC_INT_ARGS];

			/* �������� */
			xsp_vsync2(1);

			/* �X�v���C�g�\�����W */
			x = wave[frame_count * 2 % NUM_WAVE_ELEMENTS];
			y = wave[frame_count * 3 % NUM_WAVE_ELEMENTS];
			xsp_set(x + 0x80, y + 0x80, 0, 0x013F);

			/* �X�N���[�����W */ 
			arg->scroll_x = -x;
			arg->scroll_y = -y;

			/*
				�X�v���C�g���ꊇ�\������B
				�v���C�g�`��ɓ������Đݒ肷��X�N���[�����W���A
				�����A�����Ԋ��荞�݊֐��̈����Ƃ��ēn���B
			*/
			xsp_out2(arg);

			/* �t���[���J�E���g�X�V */
			frame_count++;
		}
	}

	/*-----------------------[ �I������ ]-----------------------*/

	/* XSP �̏I������ */
	xsp_off();

	/* ��ʃ��[�h��߂� */
	CRTMOD(0x10);
}




