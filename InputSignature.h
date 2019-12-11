#pragma once

struct InputSignature {
	BYTE *pIAInputSignature;        // Signature from VS or GS (if there is no VS)
								// or NULL if neither exists
	SIZE_T IAInputSignatureSize;    // Singature size in bytes   
};