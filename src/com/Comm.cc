/*
 * =============================================================================
 *
 *       Filename:  Comm.cc
 *
 *       Description: 
 *
 *       Version:  1.0
 *       Created:  04/01, 2024
 *       Revision:  V1
 *       Compiler:  g++
 *
 *       Author:  keen.lee 
 *       Company
 *
 * =============================================================================
 */

#include "com/Comm.h"
namespace litealigndata
{

    NametypeEnDeFun nametype[] = 
    {
        {"AUTO_Invalid", AUTO_Invalid, 0, NULL, NULL},
        {"AUTO_UINT64", AUTO_UINT64, sizeof(uint64_t), NULL, NULL},
        {"AUTO_INT64", AUTO_INT64, sizeof(int64_t), NULL, NULL},
        {"AUTO_UINT32", AUTO_UINT32, sizeof(uint32_t), NULL, NULL},
        {"AUTO_INT32", AUTO_INT32, sizeof(int32_t), NULL, NULL},
        {"AUTO_UINT16", AUTO_UINT16, sizeof(uint16_t), NULL, NULL},
        {"AUTO_INT16", AUTO_INT16, sizeof(int16_t), NULL, NULL},
        {"AUTO_UINT8", AUTO_UINT8, sizeof(uint8_t), NULL, NULL},
        {"AUTO_INT8", AUTO_INT8, sizeof(int8_t), NULL, NULL},
        {"AUTO_IPV4", AUTO_IPV4, 0, NULL, NULL},
        {"AUTO_IPV6", AUTO_IPV6, 0, NULL, NULL},
        {"AUTO_IPV4V6", AUTO_IPV4V6, 0, NULL, NULL},
        {"AUTO_DateTime", AUTO_DateTime, 0, NULL, NULL},
        {"AUTO_ByteArray", AUTO_ByteArray, MAX_STR_LEN, NULL, NULL},
        {"AUTO_STRING", AUTO_STRING, MAX_STR_LEN, NULL, NULL},
        {"AUTO_IMSIBCD", AUTO_IMSIBCD, 0, NULL, NULL},
        {"AUTO_BCD", AUTO_BCD, 0, NULL, NULL},
        {"AUTO_TIMEVAL", AUTO_TIMEVAL, 0, NULL, NULL},
        {"AUTO_SP_U8_4", AUTO_SP_U8_4, 0, NULL, NULL},
        {"AUTO_SP_IPV6", AUTO_SP_IPV6, 0, NULL, NULL},
        {"AUTO_IMSI_TBCD", AUTO_IMSI_TBCD, 0, NULL, NULL},
        {"AUTO_IMEISV_BCD", AUTO_IMEISV_BCD, 0, NULL, NULL},
        {"AUTO_RealTime", AUTO_RealTime, sizeof(int64_t), NULL, NULL},
        {"AUTO_BCD_2Bytes", AUTO_BCD_2Bytes, 0, NULL, NULL},
        {"AUTO_IPV6_RSA_CU", AUTO_IPV6_RSA_CU, 0, NULL, NULL},
        {"AUTO_Metric_List", AUTO_Metric_List, 0, NULL, NULL},
        {"AUTO_RealTime_ms", AUTO_RealTime_ms, sizeof(int64_t), NULL, NULL},
        {"AUTO_IPV4_IPV6", AUTO_IPV4_IPV6, 0, NULL, NULL},
        {"AUTO_SharePointer_MAC_6Byte", AUTO_SharePointer_MAC_6Byte, 0, NULL, NULL},
        {"AUTO_IPV4_NET", AUTO_IPV4_NET, 0, NULL, NULL},
        {"AUTO_IPV6_NET", AUTO_IPV6_NET, 0, NULL, NULL},
        {"AUTO_STRING_NET", AUTO_STRING_NET, 0, NULL, NULL},
        {"AUTO_STRING_NET", AUTO_STRING_NET, 0, NULL, NULL},
        {"AUTO_Struct", AUTO_STRUCT, 0, NULL, NULL},
        {"AUTO_Union", AUTO_UNION, 0, NULL, NULL},
        {"AUTO_MAX", AUTO_MAX, 0, NULL, NULL},

    };
    NametypeEnDeFun* findNametype(char* name)
    {
        for(u_int32_t i = 0; i < (sizeof(nametype))/(sizeof(NametypeEnDeFun)); i++)
        {
            if(!strcmp(name, nametype[i].name))
                return &nametype[i];
        }
        return NULL;
    };
    autoType findNameautotype(char* name)
    {
        for(u_int32_t i = 0; i < (sizeof(nametype))/(sizeof(NametypeEnDeFun)); i++)
        {
            if(!strcmp(name, nametype[i].name))
                return nametype[i].type;
        }
        return AUTO_Invalid;
    };
    uint16_t findNamelen(autoType type)
    {
        for(u_int32_t i = 0; i < (sizeof(nametype))/(sizeof(NametypeEnDeFun)); i++)
        {
            if(type == nametype[i].type)
                return nametype[i].len;
        }
        return 0;
    };
    uint16_t findNametypeLen(char* name)
    {
        for(u_int32_t i = 0; i < (sizeof(nametype))/(sizeof(NametypeEnDeFun)); i++)
        {
            if(!strcmp(name, nametype[i].name))
                return nametype[i].len;
        }
        return 0;
    };
}
// 全局类型映射表
TypeMap type_map[] = {
    {"AUTO_Char", "char", sizeof(char)},
    {"AUTO_Bit1", "union", 1},
    {"AUTO_Bit2", "union", 2},
    {"AUTO_Bit3", "union", 3},
    {"AUTO_Bit4", "union", 4},
    {"AUTO_Bit5", "union", 5},
    {"AUTO_Bit6", "union", 6},
    {"AUTO_Bit7", "union", 7},
    {"AUTO_Bit8", "union", 8},
    {"AUTO_Bit9", "union", 9},
    {"AUTO_Bit10", "union", 10},
    {"AUTO_Bit11", "union", 11},
    {"AUTO_Bit12", "union", 12},
    {"AUTO_Bit13", "union", 13},
    {"AUTO_Bit14", "union", 14},
    {"AUTO_Bit15", "union", 15},
    {"AUTO_Bit16", "union", 16},
    {"AUTO_Bit17", "union", 17},
    {"AUTO_Bit18", "union", 18},
    {"AUTO_Bit19", "union", 19},
    {"AUTO_Bit20", "union", 20},
    {"AUTO_Bit21", "union", 21},
    {"AUTO_Bit22", "union", 22},
    {"AUTO_Bit23", "union", 23},
    {"AUTO_Bit24", "union", 24},
    {"AUTO_Bit25", "union", 25},
    {"AUTO_Bit26", "union", 26},
    {"AUTO_Bit27", "union", 27},
    {"AUTO_Bit28", "union", 28},
    {"AUTO_Bit29", "union", 29},
    {"AUTO_Bit30", "union", 30},
    {"AUTO_Bit31", "union", 31},
    {"AUTO_Bit32", "union", 32},
    {"AUTO_Bit33", "union", 33},
    {"AUTO_Bit34", "union", 34},
    {"AUTO_Bit35", "union", 35},
    {"AUTO_Bit36", "union", 36},
    {"AUTO_Bit37", "union", 37},
    {"AUTO_Bit38", "union", 38},
    {"AUTO_Bit39", "union", 39},
    {"AUTO_Bit40", "union", 40},
    {"AUTO_Bit41", "union", 41},
    {"AUTO_Bit42", "union", 42},
    {"AUTO_Bit43", "union", 43},
    {"AUTO_Bit44", "union", 44},
    {"AUTO_Bit45", "union", 45},
    {"AUTO_Bit46", "union", 46},
    {"AUTO_Bit47", "union", 47},
    {"AUTO_Bit48", "union", 48},
    {"AUTO_Bit49", "union", 49},
    {"AUTO_Bit50", "union", 50},
    {"AUTO_Bit51", "union", 51},
    {"AUTO_Bit52", "union", 52},
    {"AUTO_Bit53", "union", 53},
    {"AUTO_Bit54", "union", 54},
    {"AUTO_Bit55", "union", 55},
    {"AUTO_Bit56", "union", 56},
    {"AUTO_Bit57", "union", 57},
    {"AUTO_Bit58", "union", 58},
    {"AUTO_Bit59", "union", 59},
    {"AUTO_Bit60", "union", 60},
    {"AUTO_Bit61", "union", 61},
    {"AUTO_Bit62", "union", 62},
    {"AUTO_Bit63", "union", 63},
    {"AUTO_Bit64", "union", 64},
    {"AUTO_UINT8", "unsigned int8", sizeof(uint8_t)},
    {"AUTO_INT8", "signed int8", sizeof(int8_t)},
    {"AUTO_UINT16", "unsigned int16", sizeof(uint16_t)},
    {"AUTO_INT16", "signed int16", sizeof(int16_t)},
    {"AUTO_UINT32", "unsigned int32", sizeof(uint32_t)},
    {"AUTO_INT32", "signed int32", sizeof(int32_t)},
    {"AUTO_UINT64", "unsigned long long", sizeof(uint64_t)},
    {"AUTO_UINT64", "signed long long", sizeof(int64_t)},
    {"AUTO_FLOAT", "float", sizeof(float)},
    {"AUTO_ByteArray", "char[]", 32},
    {"AUTO_Struct", "struct", 0},
    {"AUTO_Union", "union", 0},
    {NULL, NULL, 0} // 结束标记
};
// ==================== 工具函数 ====================
// 初始化动态数组
void dynamic_array_init(DynamicArray* arr) {
    arr->data = (size_t*)malloc(INIT_ARRAY_SIZE * sizeof(size_t));
    arr->size = 0;
    arr->capacity = INIT_ARRAY_SIZE;
}

// 动态数组扩容
void dynamic_array_resize(DynamicArray* arr) {
    arr->capacity *= 2;
    arr->data = (size_t*)realloc(arr->data, arr->capacity * sizeof(size_t));
}

// 动态数组添加元素
void dynamic_array_add(DynamicArray* arr, size_t value) {
    if (arr->size >= arr->capacity) {
        dynamic_array_resize(arr);
    }
    arr->data[arr->size++] = value;
}

// 释放动态数组内存
void dynamic_array_free(DynamicArray* arr) {
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

// 找动态数组最大值
size_t dynamic_array_max(DynamicArray* arr) {
    if (arr->size == 0) return 0;
    size_t max = arr->data[0];
    for (int i = 1; i < arr->size; i++) {
        if (arr->data[i] > max) {
            max = arr->data[i];
        }
    }
    return max;
}

// 初始化栈
void stack_init(Offset_Stack* stack) {
    stack->top = -1;
}

// 判断栈是否为空
int stack_is_empty(Offset_Stack* stack) {
    return stack->top == -1;
}

// 判断栈是否满
int stack_is_full(Offset_Stack* stack) {
    return stack->top == MAX_STACK_DEPTH - 1;
}

// 压栈
int stack_push(Offset_Stack* stack, NestedElement elem) {
    if (stack_is_full(stack)) {
        fprintf(stderr, "栈溢出，嵌套层级超过最大深度%d\n", MAX_STACK_DEPTH);
        return -1;
    }
    stack->data[++stack->top] = elem;
    return 0;
}

// 弹栈
NestedElement stack_pop(Offset_Stack* stack) {
    if (stack_is_empty(stack)) {
        fprintf(stderr, "栈为空，无法弹栈\n");
        NestedElement empty;//{0};
        return empty;
    }
    return stack->data[stack->top--];
}

// 获取栈顶元素
NestedElement* stack_top(Offset_Stack* stack) {
    if (stack_is_empty(stack)) {
        return NULL;
    }
    return &stack->data[stack->top];
}

// 根据XML类型查找映射信息
TypeMap* find_type_map(const char* xml_type) {
    for (int i = 0; type_map[i].xml_type != NULL; i++) {
        if (strcmp(type_map[i].xml_type, xml_type) == 0) {
            return &type_map[i];
        }
    }
    return NULL;
}