/*
	XSP ���p�T���v���v���O����

	[����]
		�v���O�������N������ƁA���������v�����Ă��܂��̂ŃL�[�{�[�h����
		���l����͂��Ă��������B�������͂������̂܂܃��^�[���L�[�������ƁA
		�f�t�H���g�l���K�p����܂��B���l�̓��͂��ς܂���ƁA�T���v���v���O
		�������J�n���܂��B

		��ʏ�ɃX�v���C�g�� 1 ���\������܂��B�W���C�X�e�B�b�N�� 8 ������
		�ړ��\�ł��B

		��ʍ����ɁA1 �t���[���̏����ɂ����������Ԃ������������׃Q�[�W��\
		�����܂��B��ʍ���ɁA�X�v���C�g�̕\�����N�G�X�g������ۂɉ�ʂɕ\
		�������܂ł̒x���t���[������\�����܂��B

		�g���K�ŁA���C�����[�v�̏������ׂ𑝌������邱�Ƃ��ł��܂��B������
		�ׂ� 1 �t���[���̋��e���Ԃ��z����Ə����������������A���e�x���t���[
		�����̃|���V�[�ɏ]���������������삪��������l�q���m�F�ł��܂��B

	[���]
		�X�v���C�g�̕\�����N�G�X�g������ۂɉ�ʂɕ\�������܂ł̒x���̉e
		�����A���ۂɃW���C�X�e�B�b�N���삵�Ȃ���̊��Ŋm�F����T���v���v��
		�O�����ł��B

		���e�x���t���[�������ǂ̂悤�ɐ��䂷�邩�A�|���V�[�Ɠ���d�l�͈ȉ�
		�̂悤�ɂȂ��Ă��܂��B

		���[�h 1 : ���e�x���t���[���� 0 �ŌŒ�
			�x���͏��Ȃ��Ȃ�܂����A������������������ƁA�t���[�����[�g��
			�����ɒቺ���܂��B

		���[�h 2 : ���e�x���t���[���� 1 �ŌŒ�
			�x���͂��傫���Ȃ�܂����A�����������������Ă��A�t���[�����[
			�g�������ɒቺ���܂���B1 �t���[����s���ĕ`�惊�N�G�X�g��~��
			�ł���̂ŁA�}���ȃt���[�����[�g�ω������艻��������ʂ������
			���B

		���[�h 3 : ���e�x���t���[�����𓮓I�ɕύX
			�����������������Ȃ����̓��[�h 1�A�����������������鎞�̓��[�h
			2 �����̓���ɂȂ�悤�ɓ��I�ɒ������܂��B���[�h 1 �̒�x���ƁA
			���[�h 2 �̃t���[�����[�g���艻�̗����̒����������ꂽ�悤��
			����ƂȂ�܂��B�������A���[�h 2 �����t���[�����[�g���艻��
			���ʂ͉�����܂��B
*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <doslib.h>
#include <iocslib.h>
#include <stdlib.h>
#include <basic0.h>
#include "../../XSP/XSP2lib.H"

int input2(char *mes, int def);

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

/* �������ׂ������邽�߃A�N�Z�X����_�~�[�ϐ� */
volatile int g_dummy = 0;

/* VSYNC �J�E���^ */
static volatile short s_vsync_count = 0;

/*------------------------[ �A�����Ԋ��荞�݊֐��ɗ^������� ]------------------------*/

/* �t���[���ԍ� */
static short s_frame_count = 0;
static volatile short s_flipped_frame_count = 0;

typedef struct {
	short scroll_x;
	short scroll_y;
	int frame_count;
} VSYNC_INT_ARG;

#define NUM_VSYNC_INT_ARGS	(4)
VSYNC_INT_ARG vsync_int_args[NUM_VSYNC_INT_ARGS] = {0};


/*----------------------------[ �R���\�[�����琔�l���� ]------------------------------*/

int input2(
	char *mes,	/* ���b�Z�[�W */
	int def		/* �f�t�H���g�l */
){
	char	str[0x100] = {0};
	int		tmp;

	printf("%s (default=%d) :", mes, def);
	tmp = atoi(gets(str));
	printf("\n");
	if (tmp == 0) tmp = def;
	return(tmp);
}


/*------------------------------[ �A�����Ԋ��荞�݊֐� ]------------------------------*/

void vsync_int(const VSYNC_INT_ARG *arg)
{
	if (arg != NULL) {
		/* VSYNC �J�E���^ */
		s_vsync_count++;

		/* �O���t�B�N�X��� 0 ��ݒ� */
		SCROLL(0, arg->scroll_x, arg->scroll_y);
		s_flipped_frame_count = arg->frame_count;
	}
}


/*-------------------------------------[ MAIN ]---------------------------------------*/

void main()
{
	int		i;
	FILE	*fp;
	int		delay_policy;

	delay_policy = input2("	�x���΍�̕��j [1]=0�t���[���Œ�, [2]=1�t���[���Œ�, [3]=��", 3);


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

	/* �ȈՐ��� */
	B_LOCATE(0, 0);
	printf(
		"   �W���C�X�e�B�b�N�ŃX�v���C�g\n"
		"   ���ړ��ł��܂��B�g���K�ŏ���\n"
		"   ���ׂ𑝌��ł��܂��B\n"
		"   �����L�[�������ƏI�����܂��B\n"
	);
	B_LOCATE(3, 5);
	printf("delay _ frames");

	/* �J�[�\������ */
	B_CUROFF();			/* X68000 EnvironmentHandBook p.312 */

	/* �O���t�B�N�X�v���[�� 0 �Ɋi�q�͗l��`�� */
	GPALET(1, 0xFFFF);
	APAGE(0);
	WINDOW(0, 0, 511, 511);
	for (i = 0; i < 512; i+=32) {
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

	/* �O���t�B�N�X�v���[�� 1 �ɏ������׃Q�[�W��`�� */
	GPALET(2, 0xFFFF);
	APAGE(1);
	WINDOW(0, 0, 511, 511);
	{
		struct FILLPTR arg;
		arg.x1 = 0;
		arg.y1 = 0;
		arg.x2 = 16;
		arg.y2 = 511;
		arg.color = 2;
		FILL(&arg);
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


	/*===========================[ �X�e�B�b�N�ő��삷��f�� ]=============================*/

	{
		/* �������� */
		int workload = 100;

		/* �L�����N�^�Ǘ��\���� */
		struct {
			short	x, y;		/* ���W */
			short	pt;			/* �X�v���C�g�p�^�[�� No. */
			short	info;		/* ���]�R�[�h�E�F�E�D��x��\���f�[�^ */
		} player;
		player.x	= 0x88;		/* X ���W�����l */
		player.y	= 0x88;		/* Y ���W�����l */
		player.pt	= 0;		/* �X�v���C�g�p�^�[�� No. */
		player.info	= 0x013F;	/* ���]�R�[�h�E�F�E�D��x��\���f�[�^ */

		/* �����L�[�������܂Ń��[�v */
		while (INPOUT(0xFF) == 0) {
			VSYNC_INT_ARG *arg;

			/* �������� */
			switch (delay_policy) {
				/* �x�� 0 �t���[���Œ� */
				case 1:{
					xsp_vsync2(0);
				} break;

				/* �x�� 1 �t���[���Œ� */
				case 2:{
					xsp_vsync2(1);
				} break;

				/* �x���t���[������ */
				case 3:{
					static short s_prev_vsync_count = 0;
					short max_delay = 0;
					if (s_prev_vsync_count == s_vsync_count) {
						/*
							�O�񂩂� 1 �t���[���̎��Ԃ��o�߂��Ă��Ȃ��B
							�`�惊�N�G�X�g���~�ς�������̂ŁAmax_delay �����Ȃ�����B
						*/
						max_delay = 0;
					} else {
						/*
							�O�񂩂� 1 �t���[���̎��Ԃ��o�߂��Ă���B
							����������������邽�߁Amax_delay ���ɘa����B
						*/
						max_delay = 1;
					}
					xsp_vsync2(max_delay);
					s_prev_vsync_count = s_vsync_count;
				} break;
			}
			GPALET(2, 0xFFFF);
			B_LOCATE(9, 5);
			printf("%d", s_frame_count - s_flipped_frame_count);

			/* �X�e�B�b�N���� */
			{
				/* �X�e�B�b�N�̓��͂ɍ����Ĉړ� */
				int	stk = JOYGET(0);
				if ((stk & 1) == 0  &&  player.y > 0x010) player.y -= 4;	/* ��Ɉړ� */
				if ((stk & 2) == 0  &&  player.y < 0x100) player.y += 4;	/* ���Ɉړ� */
				if ((stk & 4) == 0  &&  player.x > 0x010) player.x -= 4;	/* ���Ɉړ� */
				if ((stk & 8) == 0  &&  player.x < 0x100) player.x += 4;	/* �E�Ɉړ� */

				/* �g���K�ŏ������וύX */
				if ((stk & 0x20) == 0)                 workload++;			/* �g���K�[ 2 */
				if ((stk & 0x40) == 0 && workload > 0) workload--;			/* �g���K�[ 1 */
			}

			/* �������ׂ������� */
			for (i = 0; i < workload * 16; i++) {
				g_dummy++;
			}

			/* �X�v���C�g�̕\���o�^ */
			xsp_set_st(&player);

			/* �A�����Ԋ��荞�݊֐��̈����쐬 */
			arg = &vsync_int_args[s_frame_count % NUM_VSYNC_INT_ARGS];
			arg->scroll_x = s_frame_count * 2 & 511;
			arg->scroll_y = s_frame_count * 2 & 511;
			arg->frame_count = s_frame_count;

			/* �X�v���C�g���ꊇ�\������ */
			GPALET(2, 16<<11);	/* �������׃Q�[�W�ɗ΂�ݒ� */
			xsp_out2(arg);
			GPALET(2, 0);		/* �������׃Q�[�W�ɍ���ݒ� */

			/* �t���[���J�E���g�X�V */
			s_frame_count++;
		}
	}

	/* XSP �̏I������ */
	xsp_off();


	/* ��ʃ��[�h��߂� */
	CRTMOD(0x10);
}


