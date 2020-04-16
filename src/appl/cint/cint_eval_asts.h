/*
 * $Id: cint_eval_asts.h,v 1.2 2010/08/09 18:59:59 dkelley Exp $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:        cint_eval_asts.h
 * Purpose:     CINT AST evaluator interfaces
 *
 */

#ifndef __CINT_EVAL_ASTS_H__
#define __CINT_EVAL_ASTS_H__

#include "cint_config.h"
#include "cint_ast.h"
#include "cint_error.h"
#include "cint_variables.h"

/*
 * Evaluate a single AST
 */
extern cint_variable_t*
cint_eval_ast(cint_ast_t* ast); 

/*
 * Evaluate all linked ASTs and return the 
 * the value of the last one. 
 */

extern cint_variable_t*
cint_eval_asts(cint_ast_t* ast); 

#endif /* __CINT_EVAL_ASTS_H__ */
