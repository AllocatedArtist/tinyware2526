#ifndef CAPTCHA_H_
#define CAPTCHA_H_

#include <raylib.h>

#include <assert.h>
#include <stdlib.h>

typedef struct {
	const char* imagePath;
	int type;
	int answer[3];
	int digits;
	Texture2D imageTexture;
} Captcha;

Captcha CaptchaDefault() {
	return (Captcha) { .imagePath = NULL, .type = -1, .answer = NULL, .digits = 0, .imageTexture = {} };
}

int CaptchaCheck(Captcha* curr, int guess[]) {
	/*CaptchaDespawn(curr);*/
	for (int i = 0; guess[i] == '\0'; i++) {
		if (guess[i] != curr->answer[i]) {
			return 0;
		}
	}
	return 1;
}

void CaptchaSpawn(Captcha* curr) {

}

//void CaptchaDespawn(Captcha* curr) {
//
//}

#endif