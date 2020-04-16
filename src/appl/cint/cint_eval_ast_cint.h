/*
 * $Id: cint_eval_ast_cint.h,v 1.7 2011/08/31 18:30:11 dkelley Exp $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:        cint_eval_ast_cint.h
 * Purpose:     CINT AST node evaluator interfaces
 *
 */

/*
 * Interpreter Commands
 */

#ifdef CINT_AST_CINT_CMD_ENTRY

/* Load CINT data from a shared library */
CINT_AST_CINT_CMD_ENTRY(load)

/* Verify Datatypes */
CINT_AST_CINT_CMD_ENTRY(typecheck)

/* Variable Information */
CINT_AST_CINT_CMD_ENTRY(var)

/* List */
CINT_AST_CINT_CMD_ENTRY(list)

/* emit API database as YAML */
#if defined(CINT_CONFIG_YAPI)
CINT_AST_CINT_CMD_ENTRY(yapi)
#endif
    
#else



#ifndef __CINT_EVAL_AST_CINT_H__

#include "cint_variables.h"
#include "cint_ast.h"

extern cint_variable_t* 
cint_eval_ast_Cint(cint_ast_t* ast); 




#endif /* __CINT_EVAL_AST_CINT_H__ */
#endif /* CINT_AST_CINT_CMD_ENTRY */
