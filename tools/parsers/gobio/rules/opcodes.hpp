#ifndef OPCODES_HPP_HDR
#define OPCODES_HPP_HDR

struct gobio_opcodes
{
    enum opcodesE
    {
	OPCODE_ADD,
	OPCODE_ADD_CONST,
	OPCODE_ALWAYS_FALSE,
	OPCODE_ALWAYS_TRUE,
	OPCODE_ASSIGN,
	OPCODE_ASSIGN_CONST,
	OPCODE_EQUAL,
	OPCODE_EQUAL_CONST,
	OPCODE_EXTRA_ASSIGN,
	OPCODE_EXTRA_ASSIGN_CONST,
	OPCODE_EXTRA_UASSIGN,
	OPCODE_EXTRA_UASSIGN_CONST,
	OPCODE_HOOKING,
	OPCODE_HOOKING_CONST,
	OPCODE_IFJUMP,
	OPCODE_IFNJUMP,
	OPCODE_JUMP,
	OPCODE_NOP,
	OPCODE_NOT,
	OPCODE_NOT_EQUAL,
	OPCODE_NOT_EQUAL_CONST,
	OPCODE_POP,
	OPCODE_PRINT,
	OPCODE_PUSH,
	OPCODE_PUSH_EXTRA_SUBVAR,
	OPCODE_PUSH_EXTRA_VAR,
	OPCODE_PUSH_FALSE,
	OPCODE_PUSH_SUBVAR,
	OPCODE_PUSH_TRUE,
	OPCODE_PUSH_VAR,
	OPCODE_SEMANTICS_INTERSECTION,
	OPCODE_SEMANTICS_INTERSECTION_CONST,
	OPCODE_SETTOP,
	OPCODE_SETTOP_EXTRA_SUBVAR,
	OPCODE_SETTOP_EXTRA_VAR,
	OPCODE_SETTOP_SUBVAR,
	OPCODE_SETTOP_VAR,
	OPCODE_SET_SCORE,
	OPCODE_STOP,
	OPCODE_TREE_CHOICE,
	OPCODE_TRI_SEMANTICS_INTERSECTION,
	OPCODE_UASSIGN,
	OPCODE_UASSIGN_CONST,
	OPCODE_UEQUAL,
	OPCODE_UEQUAL_CONST
    };
};

#endif