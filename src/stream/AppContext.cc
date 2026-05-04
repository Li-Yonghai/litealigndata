/*
 * =============================================================================
 *
 *       Filename:  AppContext.cc
 *
 *       Description: 
 *
 *       Version:  1.0
 *       Created:  04/01, 2024
 *       Revision:  V1
 *       Compiler:  g++
 *
 *       Author:  keen.lee 
 *       Company:  
 *
 * =============================================================================
 */
#include "AppContext.h"

using namespace std;

CAppContext *CAppContext::m_instance = new CAppContext();

list<ServiceInfo> CAppContext::GetServices() const
{
    return m_services;
}
