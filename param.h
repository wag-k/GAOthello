#ifndef _PARAM_H_
#define _PARAM_H_

/* GA�Ŏg����p�����[�^���܂Ƃ߂ċL�q����t�@�C�� */
#define F_X pow((x),2)-10 /* �T������֐��B�֐��ւ̓��͂�x, �֐�����̏o�͂�y */
#define G_Y 1/(1+fabs(y))  /* �֐��̏o�͒l�ɑ΂��Ă̓K���x�B �K���x�� �����قǗǂ����̂ƈ�����Bf(x)=0���ŗ� */
#define GRAY 1             /* �O���C�R�[�h���o�C�i���R�[�h�̎w��B�O���C�R�[�h�Ȃ�1, �o�C�i���R�[�h�Ȃ�0 */

/* GA�̃p�����[�^�Q */
#define MAX (5.12)        /* ���������̍ő�l */
#define MIN (-5.12)       /* ���������̍ŏ��l */
#define LENGTH (38)        /* ��`�q�̃R�[�h�� */
#define POP 100            /* �̐� */
#define CODE_MAX 15        /* �e��`�q�R�[�h�̍ő�l�B���ꂪ�P�Ȃ�R�[�h��0��1�ɂȂ�B�r�b�g�X�g�����O�̏ꍇ�͂P�ŌŒ� */
#define GAP 0.9            /* ���̐��B�Ŏq���Ɠ���ւ�銄�� */
#define ELITE_RATE 0.8     /* ���̂܂܎c�鐔�̂����A�G���[�g�̊��� */
#define P_MUTATE 0.05      /* �ˑR�ψٗ��BLENGTH�̋t�����x���悢 */
#define P_CROSS 0.90        /* �����m�� */
#define GENERATION 15       /* GA���v�Z���鐢�㐔 */
#define SELECTION_METHOD 2 /* 1�̓��[���b�g 2�̓g�[�i�����g*/
#define TOURNAMENT_SIZE 5	/* �g�[�i�����g�T�C�Y�B�g�[�i�����g�̎������Ӗ�������  */

/* �o�� */
#define PRINT_GROUP 1
#define PRINT_FITNESS 1

#endif

