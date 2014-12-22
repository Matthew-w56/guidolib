/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class guidoengine_guidofactory */

#ifndef _Included_guidoengine_guidofactory
#define _Included_guidoengine_guidofactory
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     guidoengine_guidofactory
 * Method:    Open
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_Open
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    Close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_guidoengine_guidofactory_Close
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    OpenMusic
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_OpenMusic
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    CloseMusic
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_guidoengine_guidofactory_CloseMusic
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    OpenVoice
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_OpenVoice
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    CloseVoice
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_CloseVoice
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    OpenChord
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_OpenChord
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    CloseChord
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_CloseChord
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    InsertCommata
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_InsertCommata
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    OpenEvent
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_OpenEvent
  (JNIEnv *, jobject, jstring);

/*
 * Class:     guidoengine_guidofactory
 * Method:    CloseEvent
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_CloseEvent
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    AddSharp
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_AddSharp
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    AddFlat
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_AddFlat
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    SetEventDots
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_SetEventDots
  (JNIEnv *, jobject, jint);

/*
 * Class:     guidoengine_guidofactory
 * Method:    SetEventAccidentals
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_SetEventAccidentals
  (JNIEnv *, jobject, jint);

/*
 * Class:     guidoengine_guidofactory
 * Method:    SetOctave
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_SetOctave
  (JNIEnv *, jobject, jint);

/*
 * Class:     guidoengine_guidofactory
 * Method:    SetDuration
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_SetDuration
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     guidoengine_guidofactory
 * Method:    OpenTag
 * Signature: (Ljava/lang/String;J)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_OpenTag
  (JNIEnv *, jobject, jstring, jlong);

/*
 * Class:     guidoengine_guidofactory
 * Method:    OpenRangeTag
 * Signature: (Ljava/lang/String;J)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_OpenRangeTag
  (JNIEnv *, jobject, jstring, jlong);

/*
 * Class:     guidoengine_guidofactory
 * Method:    IsRangeTag
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_IsRangeTag
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    EndTag
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_EndTag
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    CloseTag
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_CloseTag
  (JNIEnv *, jobject);

/*
 * Class:     guidoengine_guidofactory
 * Method:    AddTagParameterString
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_AddTagParameterString
  (JNIEnv *, jobject, jstring);

/*
 * Class:     guidoengine_guidofactory
 * Method:    AddTagParameterInt
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_AddTagParameterInt
  (JNIEnv *, jobject, jint);

/*
 * Class:     guidoengine_guidofactory
 * Method:    AddTagParameterFloat
 * Signature: (D)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_AddTagParameterFloat
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     guidoengine_guidofactory
 * Method:    SetParameterName
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_SetParameterName
  (JNIEnv *, jobject, jstring);

/*
 * Class:     guidoengine_guidofactory
 * Method:    SetParameterUnit
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_guidoengine_guidofactory_SetParameterUnit
  (JNIEnv *, jobject, jstring);

/*
 * Class:     guidoengine_guidofactory
 * Method:    Init
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_guidoengine_guidofactory_Init
  (JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif
