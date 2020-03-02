/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2016 MediaTek Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
/*
** Id: //Department/DaVinci/BRANCHES/MT6620_WIFI_DRIVER_V2_3/nic/nic_cmd_event.c#3
*/

/*! \file   nic_cmd_event.c
*    \brief  Callback functions for Command packets.
*
*	Various Event packet handlers which will be setup in the callback function of
*    a command packet.
*/


/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "precomp.h"
#include "gl_ate_agent.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
const struct NIC_CAPABILITY_V2_REF_TABLE gNicCapabilityV2InfoTable[] = {
	/* {TAG_CAP_TX_RESOURCE, nicEventQueryTxResourceEntry}, */
	{TAG_CAP_TX_EFUSEADDRESS, nicCmdEventQueryNicEfuseAddr},
	{TAG_CAP_COEX_FEATURE, nicCmdEventQueryNicCoexFeature},
	{TAG_CAP_SINGLE_SKU, rlmDomainExtractSingleSkuInfoFromFirmware},
#if CFG_TCP_IP_CHKSUM_OFFLOAD
	{TAG_CAP_CSUM_OFFLOAD, nicCmdEventQueryNicCsumOffload},
#endif
	{TAG_CAP_HW_VERSION, nicCfgChipCapHwVersion},
	{TAG_CAP_SW_VERSION, nicCfgChipCapSwVersion},
	{TAG_CAP_MAC_ADDR, nicCfgChipCapMacAddr},
	{TAG_CAP_PHY_CAP, nicCfgChipCapPhyCap},
	{TAG_CAP_MAC_CAP, nicCfgChipCapMacCap},
	{TAG_CAP_FRAME_BUF_CAP, nicCfgChipCapFrameBufCap},
	{TAG_CAP_BEAMFORM_CAP, nicCfgChipCapBeamformCap},
	{TAG_CAP_LOCATION_CAP, nicCfgChipCapLocationCap},
	{TAG_CAP_MUMIMO_CAP, nicCfgChipCapMuMimoCap},
};

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                            F U N C T I O N   D A T A
********************************************************************************
*/
void nicCmdEventQueryMcrRead(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct PARAM_CUSTOM_MCR_RW_STRUCT *prMcrRdInfo;
	struct GLUE_INFO *prGlueInfo;
	struct CMD_ACCESS_REG *prCmdAccessReg;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	/* 4 <2> Update information of OID */
	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prCmdAccessReg = (struct CMD_ACCESS_REG *) (pucEventBuf);

		u4QueryInfoLen = sizeof(struct PARAM_CUSTOM_MCR_RW_STRUCT);

		prMcrRdInfo = (struct PARAM_CUSTOM_MCR_RW_STRUCT *) prCmdInfo->pvInformationBuffer;
		prMcrRdInfo->u4McrOffset = prCmdAccessReg->u4Address;
		prMcrRdInfo->u4McrData = prCmdAccessReg->u4Data;

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

	return;

}

#if CFG_SUPPORT_QA_TOOL
void nicCmdEventQueryRxStatistics(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct PARAM_CUSTOM_ACCESS_RX_STAT *prRxStatistics;
	struct EVENT_ACCESS_RX_STAT *prEventAccessRxStat;
	uint32_t u4QueryInfoLen, i;
	struct GLUE_INFO *prGlueInfo;
	uint32_t *prElement;
	uint32_t u4Temp;
	/* P_CMD_ACCESS_RX_STAT                  prCmdRxStat, prRxStat; */

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	/* 4 <2> Update information of OID */
	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prEventAccessRxStat = (struct EVENT_ACCESS_RX_STAT *) (pucEventBuf);

		prRxStatistics = (struct PARAM_CUSTOM_ACCESS_RX_STAT *) prCmdInfo->pvInformationBuffer;
		prRxStatistics->u4SeqNum = prEventAccessRxStat->u4SeqNum;
		prRxStatistics->u4TotalNum = prEventAccessRxStat->u4TotalNum;

		u4QueryInfoLen = sizeof(struct CMD_ACCESS_RX_STAT);

		if (prRxStatistics->u4SeqNum == u4RxStatSeqNum) {
			prElement = &g_HqaRxStat.MAC_FCS_Err;
			for (i = 0; i < HQA_RX_STATISTIC_NUM; i++) {
				u4Temp = ntohl(prEventAccessRxStat->au4Buffer[i]);
				kalMemCopy(prElement, &u4Temp, 4);

				if (i < (HQA_RX_STATISTIC_NUM - 1))
					prElement++;
			}

			g_HqaRxStat.AllMacMdrdy0 = ntohl(prEventAccessRxStat->au4Buffer[i]);
			i++;
			g_HqaRxStat.AllMacMdrdy1 = ntohl(prEventAccessRxStat->au4Buffer[i]);
			/* i++; */
			/* g_HqaRxStat.AllFCSErr0 = ntohl(prEventAccessRxStat->au4Buffer[i]); */
			/* i++; */
			/* g_HqaRxStat.AllFCSErr1 = ntohl(prEventAccessRxStat->au4Buffer[i]); */
		}

		DBGLOG(INIT, ERROR,
			"MT6632 : RX Statistics Test SeqNum = %d, TotalNum = %d\n",
			 (unsigned int)prEventAccessRxStat->u4SeqNum, (unsigned int)prEventAccessRxStat->u4TotalNum);

		DBGLOG(INIT, ERROR, "MAC_FCS_ERR = %d, MAC_MDRDY = %d, MU_RX_CNT = %d, RX_FIFO_FULL = %d\n",
			 (unsigned int)prEventAccessRxStat->au4Buffer[0],
				(unsigned int)prEventAccessRxStat->au4Buffer[1],
				(unsigned int)prEventAccessRxStat->au4Buffer[65],
				(unsigned int)prEventAccessRxStat->au4Buffer[22]);

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

}

#if CFG_SUPPORT_TX_BF
void nicCmdEventPfmuDataRead(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct GLUE_INFO *prGlueInfo;
	union PFMU_DATA *prEventPfmuDataRead = NULL;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	/* 4 <2> Update information of OID */
	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prEventPfmuDataRead = (union PFMU_DATA *) (pucEventBuf);

		u4QueryInfoLen = sizeof(union PFMU_DATA);

		g_rPfmuData = *prEventPfmuDataRead;

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

	DBGLOG(INIT, INFO, "=========== Before ===========\n");
	if (prEventPfmuDataRead != NULL) {
		DBGLOG(INIT, INFO, "u2Phi11 = 0x%x\n", prEventPfmuDataRead->rField.u2Phi11);
		DBGLOG(INIT, INFO, "ucPsi21 = 0x%x\n", prEventPfmuDataRead->rField.ucPsi21);
		DBGLOG(INIT, INFO, "u2Phi21 = 0x%x\n", prEventPfmuDataRead->rField.u2Phi21);
		DBGLOG(INIT, INFO, "ucPsi31 = 0x%x\n", prEventPfmuDataRead->rField.ucPsi31);
		DBGLOG(INIT, INFO, "u2Phi31 = 0x%x\n", prEventPfmuDataRead->rField.u2Phi31);
		DBGLOG(INIT, INFO, "ucPsi41 = 0x%x\n", prEventPfmuDataRead->rField.ucPsi41);
		DBGLOG(INIT, INFO, "u2Phi22 = 0x%x\n", prEventPfmuDataRead->rField.u2Phi22);
		DBGLOG(INIT, INFO, "ucPsi32 = 0x%x\n", prEventPfmuDataRead->rField.ucPsi32);
		DBGLOG(INIT, INFO, "u2Phi32 = 0x%x\n", prEventPfmuDataRead->rField.u2Phi32);
		DBGLOG(INIT, INFO, "ucPsi42 = 0x%x\n", prEventPfmuDataRead->rField.ucPsi42);
		DBGLOG(INIT, INFO, "u2Phi33 = 0x%x\n", prEventPfmuDataRead->rField.u2Phi33);
		DBGLOG(INIT, INFO, "ucPsi43 = 0x%x\n", prEventPfmuDataRead->rField.ucPsi43);
		DBGLOG(INIT, INFO, "u2dSNR00 = 0x%x\n", prEventPfmuDataRead->rField.u2dSNR00);
		DBGLOG(INIT, INFO, "u2dSNR01 = 0x%x\n", prEventPfmuDataRead->rField.u2dSNR01);
		DBGLOG(INIT, INFO, "u2dSNR02 = 0x%x\n", prEventPfmuDataRead->rField.u2dSNR02);
		DBGLOG(INIT, INFO, "u2dSNR03 = 0x%x\n", prEventPfmuDataRead->rField.u2dSNR03);
	}
}

void nicCmdEventPfmuTagRead(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct GLUE_INFO *prGlueInfo;
	struct EVENT_PFMU_TAG_READ *prEventPfmuTagRead = NULL;
	struct PARAM_CUSTOM_PFMU_TAG_READ_STRUCT *prPfumTagRead = NULL;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	if (!pucEventBuf) {
		DBGLOG(INIT, ERROR, "pucEventBuf is NULL.\n");
		return;
	}
	if (!prCmdInfo->pvInformationBuffer) {
		DBGLOG(INIT, ERROR, "prCmdInfo->pvInformationBuffer is NULL.\n");
		return;
	}
	/* 4 <2> Update information of OID */
	if (!prCmdInfo->fgIsOid) {
		DBGLOG(INIT, ERROR, "cmd %u seq #%u not oid!",
		       prCmdInfo->ucCID, prCmdInfo->ucCmdSeqNum);
		return;
	}
	prGlueInfo = prAdapter->prGlueInfo;
	prEventPfmuTagRead = (struct EVENT_PFMU_TAG_READ *) (pucEventBuf);

	prPfumTagRead = (struct PARAM_CUSTOM_PFMU_TAG_READ_STRUCT *) prCmdInfo->pvInformationBuffer;

	kalMemCopy(prPfumTagRead, prEventPfmuTagRead, sizeof(struct EVENT_PFMU_TAG_READ));

	u4QueryInfoLen = sizeof(union CMD_TXBF_ACTION);

	g_rPfmuTag1 = prPfumTagRead->ru4TxBfPFMUTag1;
	g_rPfmuTag2 = prPfumTagRead->ru4TxBfPFMUTag2;

	kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);

	DBGLOG(INIT, INFO, "========================== (R)Tag1 info ==========================\n");

	DBGLOG(INIT, INFO, " Row data0 : %x, Row data1 : %x, Row data2 : %x, Row data3 : %x\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.au4RawData[0],
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.au4RawData[1],
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.au4RawData[2], prEventPfmuTagRead->ru4TxBfPFMUTag1.au4RawData[3]);
	DBGLOG(INIT, INFO, "ProfileID = %d Invalid status = %d\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucProfileID,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucInvalidProf);
	DBGLOG(INIT, INFO, "0:iBF / 1:eBF = %d\n", prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucTxBf);
	DBGLOG(INIT, INFO, "0:SU / 1:MU = %d\n", prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucSU_MU);
	DBGLOG(INIT, INFO, "DBW(0/1/2/3 BW20/40/80/160NC) = %d\n", prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucDBW);
	DBGLOG(INIT, INFO, "RMSD = %d\n", prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucRMSD);
	DBGLOG(INIT, INFO, "Nrow = %d, Ncol = %d, Ng = %d, LM = %d\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucNrow,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucNcol,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucNgroup, prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucLM);
	DBGLOG(INIT, INFO, "Mem1(%d, %d), Mem2(%d, %d), Mem3(%d, %d), Mem4(%d, %d)\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucMemAddr1ColIdx,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucMemAddr1RowIdx,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucMemAddr2ColIdx,
	       (prEventPfmuTagRead->ru4TxBfPFMUTag1.
		rField.ucMemAddr2RowIdx | (prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucMemAddr2RowIdxMsb << 5)),
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucMemAddr3ColIdx,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucMemAddr3RowIdx,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucMemAddr4ColIdx,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucMemAddr4RowIdx);
	DBGLOG(INIT, INFO, "SNR STS0=0x%x, SNR STS1=0x%x, SNR STS2=0x%x, SNR STS3=0x%x\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucSNR_STS0,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucSNR_STS1,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucSNR_STS2,
	       prEventPfmuTagRead->ru4TxBfPFMUTag1.rField.ucSNR_STS3);
	DBGLOG(INIT, INFO, "===============================================================\n");

	DBGLOG(INIT, INFO, "========================== (R)Tag2 info ==========================\n");
	DBGLOG(INIT, INFO, " Row data0 : %x, Row data1 : %x, Row data2 : %x\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.au4RawData[0],
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.au4RawData[1], prEventPfmuTagRead->ru4TxBfPFMUTag2.au4RawData[2]);
	DBGLOG(INIT, INFO, "Smart Ant Cfg = %d\n", prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.u2SmartAnt);
	DBGLOG(INIT, INFO, "SE index = %d\n", prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.ucSEIdx);
	DBGLOG(INIT, INFO, "RMSD Threshold = %d\n", prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.ucRMSDThd);
	DBGLOG(INIT, INFO, "MCS TH L1SS = %d, S1SS = %d, L2SS = %d, S2SS = %d\n"
	       "L3SS = %d, S3SS = %d\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.ucMCSThL1SS,
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.ucMCSThS1SS,
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.ucMCSThL2SS,
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.ucMCSThS2SS,
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.ucMCSThL3SS,
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.ucMCSThS3SS);
	DBGLOG(INIT, INFO, "iBF lifetime limit(unit:4ms) = 0x%x\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.uciBfTimeOut);
	DBGLOG(INIT, INFO, "iBF desired DBW = %d\n  0/1/2/3 : BW20/40/80/160NC\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.uciBfDBW);
	DBGLOG(INIT, INFO, "iBF desired Ncol = %d\n  0/1/2 : Ncol = 1 ~ 3\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.uciBfNcol);
	DBGLOG(INIT, INFO, "iBF desired Nrow = %d\n  0/1/2/3 : Nrow = 1 ~ 4\n",
	       prEventPfmuTagRead->ru4TxBfPFMUTag2.rField.uciBfNrow);
	DBGLOG(INIT, INFO, "===============================================================\n");

}

#endif /* CFG_SUPPORT_TX_BF */
#if CFG_SUPPORT_MU_MIMO
void nicCmdEventGetQd(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct GLUE_INFO *prGlueInfo;
	struct EVENT_HQA_GET_QD *prEventHqaGetQd;
	uint32_t i;

	struct PARAM_CUSTOM_GET_QD_STRUCT *prGetQd = NULL;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);
	if (!pucEventBuf) {
		DBGLOG(INIT, ERROR, "pucEventBuf is NULL.\n");
		return;
	}
	if (!prCmdInfo->pvInformationBuffer) {
		DBGLOG(INIT, ERROR, "prCmdInfo->pvInformationBuffer is NULL.\n");
		return;
	}
	/* 4 <2> Update information of OID */
	if (!prCmdInfo->fgIsOid) {
		DBGLOG(INIT, ERROR, "cmd %u seq #%u not oid!\n",
		       prCmdInfo->ucCID, prCmdInfo->ucCmdSeqNum);
		return;
	}
	prGlueInfo = prAdapter->prGlueInfo;
	prEventHqaGetQd = (struct EVENT_HQA_GET_QD *) (pucEventBuf);

	prGetQd = (struct PARAM_CUSTOM_GET_QD_STRUCT *) prCmdInfo->pvInformationBuffer;

	kalMemCopy(prGetQd, prEventHqaGetQd, sizeof(struct EVENT_HQA_GET_QD));

	u4QueryInfoLen = sizeof(union CMD_MUMIMO_ACTION);

	/* g_rPfmuTag1 = prPfumTagRead->ru4TxBfPFMUTag1; */
	/* g_rPfmuTag2 = prPfumTagRead->ru4TxBfPFMUTag2; */

	kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);

	DBGLOG(INIT, INFO, " event id : %x\n", prGetQd->u4EventId);
	for (i = 0; i < 14; i++)
		DBGLOG(INIT, INFO, "au4RawData[%d]: %x\n", i, prGetQd->au4RawData[i]);

}

void nicCmdEventGetCalcLq(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct GLUE_INFO *prGlueInfo;
	struct EVENT_HQA_GET_MU_CALC_LQ *prEventHqaGetMuCalcLq;
	uint32_t i, j;

	struct PARAM_CUSTOM_GET_MU_CALC_LQ_STRUCT *prGetMuCalcLq = NULL;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);
	if (!pucEventBuf) {
		DBGLOG(INIT, ERROR, "pucEventBuf is NULL.\n");
		return;
	}
	if (!prCmdInfo->pvInformationBuffer) {
		DBGLOG(INIT, ERROR, "prCmdInfo->pvInformationBuffer is NULL.\n");
		return;
	}
	/* 4 <2> Update information of OID */
	if (!prCmdInfo->fgIsOid) {
		DBGLOG(INIT, ERROR, "cmd %u seq #%u not oid!\n",
		       prCmdInfo->ucCID, prCmdInfo->ucCmdSeqNum);
		return;
	}
	prGlueInfo = prAdapter->prGlueInfo;
	prEventHqaGetMuCalcLq = (struct EVENT_HQA_GET_MU_CALC_LQ *) (pucEventBuf);

	prGetMuCalcLq = (struct PARAM_CUSTOM_GET_MU_CALC_LQ_STRUCT *) prCmdInfo->pvInformationBuffer;

	kalMemCopy(prGetMuCalcLq, prEventHqaGetMuCalcLq, sizeof(struct EVENT_HQA_GET_MU_CALC_LQ));

	u4QueryInfoLen = sizeof(union CMD_MUMIMO_ACTION);

	/* g_rPfmuTag1 = prPfumTagRead->ru4TxBfPFMUTag1; */
	/* g_rPfmuTag2 = prPfumTagRead->ru4TxBfPFMUTag2; */

	kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);


	DBGLOG(INIT, INFO, " event id : %x\n", prGetMuCalcLq->u4EventId);
	for (i = 0; i < NUM_OF_USER; i++)
		for (j = 0; j < NUM_OF_MODUL; j++)
			DBGLOG(INIT, INFO, " lq_report[%d][%d]: %x\n", i, j, prGetMuCalcLq->rEntry.lq_report[i][j]);

}

void nicCmdEventGetCalcInitMcs(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct GLUE_INFO *prGlueInfo;
	struct EVENT_SHOW_GROUP_TBL_ENTRY *prEventShowGroupTblEntry = NULL;

	struct PARAM_CUSTOM_SHOW_GROUP_TBL_ENTRY_STRUCT *prShowGroupTbl;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);
	if (!pucEventBuf) {
		DBGLOG(INIT, ERROR, "pucEventBuf is NULL.\n");
		return;
	}
	if (!prCmdInfo->pvInformationBuffer) {
		DBGLOG(INIT, ERROR, "prCmdInfo->pvInformationBuffer is NULL.\n");
		return;
	}
	/* 4 <2> Update information of OID */
	if (!prCmdInfo->fgIsOid) {
		DBGLOG(INIT, ERROR, "cmd %u seq #%u not oid!\n",
		       prCmdInfo->ucCID, prCmdInfo->ucCmdSeqNum);
		return;
	}
	prGlueInfo = prAdapter->prGlueInfo;
	prEventShowGroupTblEntry = (struct EVENT_SHOW_GROUP_TBL_ENTRY *) (pucEventBuf);

	prShowGroupTbl = (struct PARAM_CUSTOM_SHOW_GROUP_TBL_ENTRY_STRUCT *) prCmdInfo->pvInformationBuffer;

	kalMemCopy(prShowGroupTbl, prEventShowGroupTblEntry, sizeof(struct EVENT_SHOW_GROUP_TBL_ENTRY));

	u4QueryInfoLen = sizeof(union CMD_MUMIMO_ACTION);

	/* g_rPfmuTag1 = prPfumTagRead->ru4TxBfPFMUTag1; */
	/* g_rPfmuTag2 = prPfumTagRead->ru4TxBfPFMUTag2; */

	kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);


	DBGLOG(INIT, INFO, "========================== (R)Group table info ==========================\n");
	DBGLOG(INIT, INFO, " event id : %x\n", prEventShowGroupTblEntry->u4EventId);
	DBGLOG(INIT, INFO, "index = %x numUser = %x\n", prEventShowGroupTblEntry->index,
	       prEventShowGroupTblEntry->numUser);
	DBGLOG(INIT, INFO, "BW = %x NS0/1/ = %x/%x\n", prEventShowGroupTblEntry->BW, prEventShowGroupTblEntry->NS0,
	       prEventShowGroupTblEntry->NS1);
	DBGLOG(INIT, INFO, "PFIDUser0/1 = %x/%x\n", prEventShowGroupTblEntry->PFIDUser0,
	       prEventShowGroupTblEntry->PFIDUser1);
	DBGLOG(INIT, INFO, "fgIsShortGI = %x, fgIsUsed = %x, fgIsDisable = %x\n", prEventShowGroupTblEntry->fgIsShortGI,
	       prEventShowGroupTblEntry->fgIsUsed, prEventShowGroupTblEntry->fgIsDisable);
	DBGLOG(INIT, INFO, "initMcsUser0/1 = %x/%x\n", prEventShowGroupTblEntry->initMcsUser0,
	       prEventShowGroupTblEntry->initMcsUser1);
	DBGLOG(INIT, INFO, "dMcsUser0: 0/1/ = %x/%x\n", prEventShowGroupTblEntry->dMcsUser0,
	       prEventShowGroupTblEntry->dMcsUser1);

}
#endif /* CFG_SUPPORT_MU_MIMO */
#endif /* CFG_SUPPORT_QA_TOOL */

void nicCmdEventQuerySwCtrlRead(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct PARAM_CUSTOM_SW_CTRL_STRUCT *prSwCtrlInfo;
	struct GLUE_INFO *prGlueInfo;
	struct CMD_SW_DBG_CTRL *prCmdSwCtrl;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	/* 4 <2> Update information of OID */
	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prCmdSwCtrl = (struct CMD_SW_DBG_CTRL *) (pucEventBuf);

		u4QueryInfoLen = sizeof(struct PARAM_CUSTOM_SW_CTRL_STRUCT);

		prSwCtrlInfo = (struct PARAM_CUSTOM_SW_CTRL_STRUCT *) prCmdInfo->pvInformationBuffer;
		prSwCtrlInfo->u4Id = prCmdSwCtrl->u4Id;
		prSwCtrlInfo->u4Data = prCmdSwCtrl->u4Data;

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

}

void nicCmdEventQueryChipConfig(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct PARAM_CUSTOM_CHIP_CONFIG_STRUCT *prChipConfigInfo;
	struct GLUE_INFO *prGlueInfo;
	struct CMD_CHIP_CONFIG *prCmdChipConfig;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	/* 4 <2> Update information of OID */
	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prCmdChipConfig = (struct CMD_CHIP_CONFIG *) (pucEventBuf);

		u4QueryInfoLen = sizeof(struct PARAM_CUSTOM_CHIP_CONFIG_STRUCT);

		if (prCmdInfo->u4InformationBufferLength < sizeof(struct PARAM_CUSTOM_CHIP_CONFIG_STRUCT)) {
			DBGLOG(REQ, INFO,
			       "Chip config u4InformationBufferLength %u is not valid (event)\n",
			       prCmdInfo->u4InformationBufferLength);
		}
		prChipConfigInfo = (struct PARAM_CUSTOM_CHIP_CONFIG_STRUCT *) prCmdInfo->pvInformationBuffer;
		prChipConfigInfo->ucRespType = prCmdChipConfig->ucRespType;
		prChipConfigInfo->u2MsgSize = prCmdChipConfig->u2MsgSize;
		DBGLOG(REQ, INFO, "%s: RespTyep  %u\n", __func__, prChipConfigInfo->ucRespType);
		DBGLOG(REQ, INFO, "%s: u2MsgSize %u\n", __func__, prChipConfigInfo->u2MsgSize);

#if 0
		if (prChipConfigInfo->u2MsgSize > CHIP_CONFIG_RESP_SIZE) {
			DBGLOG(REQ, INFO,
			       "Chip config Msg Size %u is not valid (event)\n", prChipConfigInfo->u2MsgSize);
			prChipConfigInfo->u2MsgSize = CHIP_CONFIG_RESP_SIZE;
		}
#endif
		kalMemCopy(prChipConfigInfo->aucCmd, prCmdChipConfig->aucCmd, prChipConfigInfo->u2MsgSize);
		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

}

void nicCmdEventSetCommon(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	if (prCmdInfo->fgIsOid) {
		/* Update Set Information Length */
		kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery,
			       prCmdInfo->u4InformationBufferLength, WLAN_STATUS_SUCCESS);
	}

}

void nicCmdEventSetDisassociate(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	if (prCmdInfo->fgIsOid) {
		/* Update Set Information Length */
		kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
	}

	kalIndicateStatusAndComplete(prAdapter->prGlueInfo, WLAN_STATUS_MEDIA_DISCONNECT, NULL, 0);

#if !defined(LINUX)
	prAdapter->fgIsRadioOff = TRUE;
#endif

}

void nicCmdEventSetIpAddress(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4Count;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	u4Count = (prCmdInfo->u4SetInfoLen - OFFSET_OF(struct CMD_SET_NETWORK_ADDRESS_LIST, arNetAddress))
	    / sizeof(struct IPV4_NETWORK_ADDRESS);

	if (prCmdInfo->fgIsOid) {
		/* Update Set Information Length */
		kalOidComplete(prAdapter->prGlueInfo,
			       prCmdInfo->fgSetQuery,
			       OFFSET_OF(struct PARAM_NETWORK_ADDRESS_LIST, arAddress) + u4Count *
			       (OFFSET_OF(struct PARAM_NETWORK_ADDRESS, aucAddress) +
				sizeof(struct PARAM_NETWORK_ADDRESS_IP)), WLAN_STATUS_SUCCESS);
	}

}

void nicCmdEventQueryRfTestATInfo(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	union EVENT_TEST_STATUS *prTestStatus, *prQueryBuffer;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prTestStatus = (union EVENT_TEST_STATUS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prQueryBuffer = (union EVENT_TEST_STATUS *) prCmdInfo->pvInformationBuffer;

		/* Memory copy length is depended on upper-layer */
		kalMemCopy(prQueryBuffer, prTestStatus, prCmdInfo->u4InformationBufferLength);

		u4QueryInfoLen = sizeof(union EVENT_TEST_STATUS);

		/* Update Query Information Length */
		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

}

void nicCmdEventQueryLinkQuality(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	int32_t rRssi, *prRssi;
	struct EVENT_LINK_QUALITY *prLinkQuality;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prLinkQuality = (struct EVENT_LINK_QUALITY *) pucEventBuf;

	rRssi = (int32_t) prLinkQuality->cRssi;	/* ranged from (-128 ~ 30) in unit of dBm */

	if (prAdapter->prAisBssInfo->eConnectionState == PARAM_MEDIA_STATE_CONNECTED) {
		if (rRssi > PARAM_WHQL_RSSI_MAX_DBM)
			rRssi = PARAM_WHQL_RSSI_MAX_DBM;
		else if (rRssi < PARAM_WHQL_RSSI_MIN_DBM)
			rRssi = PARAM_WHQL_RSSI_MIN_DBM;
	} else {
		rRssi = PARAM_WHQL_RSSI_MIN_DBM;
	}

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prRssi = (int32_t *) prCmdInfo->pvInformationBuffer;

		kalMemCopy(prRssi, &rRssi, sizeof(int32_t));
		u4QueryInfoLen = sizeof(int32_t);

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This routine is in response of OID_GEN_LINK_SPEED query request
*
* @param prAdapter      Pointer to the Adapter structure.
* @param prCmdInfo      Pointer to the pending command info
* @param pucEventBuf
*
* @retval none
*/
/*----------------------------------------------------------------------------*/
void nicCmdEventQueryLinkSpeed(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_LINK_QUALITY *prLinkQuality;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4LinkSpeed;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prLinkQuality = (struct EVENT_LINK_QUALITY *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		pu4LinkSpeed = (uint32_t *) (prCmdInfo->pvInformationBuffer);

		*pu4LinkSpeed = prLinkQuality->u2LinkSpeed * 5000;

		u4QueryInfoLen = sizeof(uint32_t);

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryStatistics(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct PARAM_802_11_STATISTICS_STRUCT *prStatistics;
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		u4QueryInfoLen = sizeof(struct PARAM_802_11_STATISTICS_STRUCT);
		prStatistics = (struct PARAM_802_11_STATISTICS_STRUCT *) prCmdInfo->pvInformationBuffer;

		prStatistics->rTransmittedFragmentCount = prEventStatistics->rTransmittedFragmentCount;
		prStatistics->rFailedCount = prEventStatistics->rFailedCount;
		prStatistics->rRetryCount = prEventStatistics->rRetryCount;
		prStatistics->rMultipleRetryCount = prEventStatistics->rMultipleRetryCount;
		prStatistics->rACKFailureCount = prEventStatistics->rACKFailureCount;
		prStatistics->rReceivedFragmentCount = prEventStatistics->rReceivedFragmentCount;
		prStatistics->rFCSErrorCount = prEventStatistics->rFCSErrorCount;

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryBugReport(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
#define BUG_REPORT_VERSION 1

	struct _EVENT_BUG_REPORT_T *prStatistics;
	struct _EVENT_BUG_REPORT_T *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct _EVENT_BUG_REPORT_T *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		u4QueryInfoLen = sizeof(struct _EVENT_BUG_REPORT_T);
		if (prEventStatistics->u4BugReportVersion == BUG_REPORT_VERSION) {
			prStatistics = (struct _EVENT_BUG_REPORT_T *) prCmdInfo->pvInformationBuffer;
			kalMemCopy(prStatistics, prEventStatistics, u4QueryInfoLen);
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventEnterRfTest(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	/* [driver-land] */
	/* prAdapter->fgTestMode = TRUE; */
	if (prAdapter->fgTestMode)
		prAdapter->fgTestMode = FALSE;
	else
		prAdapter->fgTestMode = TRUE;

	/* 0. always indicate disconnection */
	if (kalGetMediaStateIndicated(prAdapter->prGlueInfo) != PARAM_MEDIA_STATE_DISCONNECTED)
		kalIndicateStatusAndComplete(prAdapter->prGlueInfo, WLAN_STATUS_MEDIA_DISCONNECT, NULL, 0);
	/* 1. Remove pending TX */
	nicTxRelease(prAdapter, TRUE);

	/* 1.1 clear pending Security / Management Frames */
	kalClearSecurityFrames(prAdapter->prGlueInfo);
	kalClearMgmtFrames(prAdapter->prGlueInfo);

	/* 1.2 clear pending TX packet queued in glue layer */
	kalFlushPendingTxPackets(prAdapter->prGlueInfo);

	/* 2. Reset driver-domain FSMs */
	nicUninitMGMT(prAdapter);

	nicResetSystemService(prAdapter);
	nicInitMGMT(prAdapter, NULL);

	/* Block til firmware completed entering into RF test mode */
	kalMsleep(500);

#if defined(_HIF_SDIO) && 0
	/* 3. Disable Interrupt */
	HAL_INTR_DISABLE(prAdapter);

	/* 4. Block til firmware completed entering into RF test mode */
	kalMsleep(500);
	while (1) {
		uint32_t u4Value;

		HAL_MCR_RD(prAdapter, MCR_WCIR, &u4Value);

		if (u4Value & WCIR_WLAN_READY) {
			break;
		} else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE || fgIsBusAccessFailed == TRUE) {
			if (prCmdInfo->fgIsOid) {
				/* Update Set Information Length */
				kalOidComplete(prAdapter->prGlueInfo,
					       prCmdInfo->fgSetQuery,
					       prCmdInfo->u4SetInfoLen, WLAN_STATUS_NOT_SUPPORTED);

			}
			return;
		}
			kalMsleep(10);
	}

	/* 5. Clear Interrupt Status */
	{
		uint32_t u4WHISR = 0;
		uint16_t au2TxCount[16];

		HAL_READ_INTR_STATUS(prAdapter, 4, (uint8_t *)&u4WHISR);
		if (HAL_IS_TX_DONE_INTR(u4WHISR))
			HAL_READ_TX_RELEASED_COUNT(prAdapter, au2TxCount);
	}
	/* 6. Reset TX Counter */
	nicTxResetResource(prAdapter);

	/* 7. Re-enable Interrupt */
	HAL_INTR_ENABLE(prAdapter);
#endif

	/* 8. completion indication */
	if (prCmdInfo->fgIsOid) {
		/* Update Set Information Length */
		kalOidComplete(prAdapter->prGlueInfo,
			       prCmdInfo->fgSetQuery, prCmdInfo->u4SetInfoLen, WLAN_STATUS_SUCCESS);
	}
#if CFG_SUPPORT_NVRAM
	/* 9. load manufacture data */
	if (kalIsConfigurationExist(prAdapter->prGlueInfo) == TRUE)
		wlanLoadManufactureData(prAdapter, kalGetConfiguration(prAdapter->prGlueInfo));
	else
		DBGLOG(REQ, WARN, "%s: load manufacture data fail\n", __func__);
#endif

}

void nicCmdEventLeaveRfTest(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	/* Block until firmware completed leaving from RF test mode */
	kalMsleep(500);

#if defined(_HIF_SDIO) && 0
	uint32_t u4WHISR = 0;
	uint16_t au2TxCount[16];
	uint32_t u4Value;

	/* 1. Disable Interrupt */
	HAL_INTR_DISABLE(prAdapter);

	/* 2. Block until firmware completed leaving from RF test mode */
	kalMsleep(500);
	while (1) {
		HAL_MCR_RD(prAdapter, MCR_WCIR, &u4Value);

		if (u4Value & WCIR_WLAN_READY) {
			break;
		} else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE || fgIsBusAccessFailed == TRUE) {
			if (prCmdInfo->fgIsOid) {
				/* Update Set Information Length */
				kalOidComplete(prAdapter->prGlueInfo,
					       prCmdInfo->fgSetQuery,
					       prCmdInfo->u4SetInfoLen, WLAN_STATUS_NOT_SUPPORTED);

			}
			return;
		}
			kalMsleep(10);
	}
	/* 3. Clear Interrupt Status */
	HAL_READ_INTR_STATUS(prAdapter, 4, (uint8_t *)&u4WHISR);
	if (HAL_IS_TX_DONE_INTR(u4WHISR))
		HAL_READ_TX_RELEASED_COUNT(prAdapter, au2TxCount);
	/* 4. Reset TX Counter */
	nicTxResetResource(prAdapter);

	/* 5. Re-enable Interrupt */
	HAL_INTR_ENABLE(prAdapter);
#endif

	/* 6. set driver-land variable */
	prAdapter->fgTestMode = FALSE;
	prAdapter->fgIcapMode = FALSE;

	/* 7. completion indication */
	if (prCmdInfo->fgIsOid) {
		/* Update Set Information Length */
		kalOidComplete(prAdapter->prGlueInfo,
			       prCmdInfo->fgSetQuery, prCmdInfo->u4SetInfoLen, WLAN_STATUS_SUCCESS);
	}

	/* 8. Indicate as disconnected */
	if (kalGetMediaStateIndicated(prAdapter->prGlueInfo) != PARAM_MEDIA_STATE_DISCONNECTED) {

		kalIndicateStatusAndComplete(prAdapter->prGlueInfo, WLAN_STATUS_MEDIA_DISCONNECT, NULL, 0);

		prAdapter->rWlanInfo.u4SysTime = kalGetTimeTick();
	}
#if CFG_SUPPORT_NVRAM
	/* 9. load manufacture data */
	if (kalIsConfigurationExist(prAdapter->prGlueInfo) == TRUE)
		wlanLoadManufactureData(prAdapter, kalGetConfiguration(prAdapter->prGlueInfo));
	else
		DBGLOG(REQ, WARN, "%s: load manufacture data fail\n", __func__);
#endif

	/* 10. Override network address */
	wlanUpdateNetworkAddress(prAdapter);

}

void nicCmdEventQueryMcastAddr(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct GLUE_INFO *prGlueInfo;
	struct CMD_MAC_MCAST_ADDR *prEventMacMcastAddr;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	/* 4 <2> Update information of OID */
	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prEventMacMcastAddr = (struct CMD_MAC_MCAST_ADDR *) (pucEventBuf);

		u4QueryInfoLen = prEventMacMcastAddr->u4NumOfGroupAddr * MAC_ADDR_LEN;

		/* buffer length check */
		if (prCmdInfo->u4InformationBufferLength < u4QueryInfoLen) {
			kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_BUFFER_TOO_SHORT);
		} else {
			kalMemCopy(prCmdInfo->pvInformationBuffer,
				   prEventMacMcastAddr->arAddress,
				   prEventMacMcastAddr->u4NumOfGroupAddr * MAC_ADDR_LEN);

			kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
		}
	}
}

void nicCmdEventQueryEepromRead(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct PARAM_CUSTOM_EEPROM_RW_STRUCT *prEepromRdInfo;
	struct GLUE_INFO *prGlueInfo;
	struct CMD_ACCESS_EEPROM *prEventAccessEeprom;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	/* 4 <2> Update information of OID */
	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prEventAccessEeprom = (struct CMD_ACCESS_EEPROM *) (pucEventBuf);

		u4QueryInfoLen = sizeof(struct PARAM_CUSTOM_EEPROM_RW_STRUCT);

		prEepromRdInfo = (struct PARAM_CUSTOM_EEPROM_RW_STRUCT *) prCmdInfo->pvInformationBuffer;
		prEepromRdInfo->ucEepromIndex = (uint8_t) (prEventAccessEeprom->u2Offset);
		prEepromRdInfo->u2EepromData = prEventAccessEeprom->u2Data;

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

}

void nicCmdEventSetMediaStreamMode(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct PARAM_MEDIA_STREAMING_INDICATION rParamMediaStreamIndication;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	if (prCmdInfo->fgIsOid) {
		/* Update Set Information Length */
		kalOidComplete(prAdapter->prGlueInfo,
			       prCmdInfo->fgSetQuery, prCmdInfo->u4SetInfoLen, WLAN_STATUS_SUCCESS);
	}

	rParamMediaStreamIndication.rStatus.eStatusType = ENUM_STATUS_TYPE_MEDIA_STREAM_MODE;
	rParamMediaStreamIndication.eMediaStreamMode =
	    prAdapter->rWlanInfo.eLinkAttr.ucMediaStreamMode == 0 ? ENUM_MEDIA_STREAM_OFF : ENUM_MEDIA_STREAM_ON;

	kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
				     WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
				     (void *)&rParamMediaStreamIndication, sizeof(struct PARAM_MEDIA_STREAMING_INDICATION));
}

void nicCmdEventSetStopSchedScan(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	/*
	 *  DBGLOG(SCN, INFO, "--->nicCmdEventSetStopSchedScan\n" ));
	 */
	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	/*
	 *  DBGLOG(SCN, INFO, "<--kalSchedScanStopped\n" );
	 */
	if (prCmdInfo->fgIsOid) {
		/* Update Set Information Length */
		kalOidComplete(prAdapter->prGlueInfo,
			       prCmdInfo->fgSetQuery, prCmdInfo->u4InformationBufferLength, WLAN_STATUS_SUCCESS);
	}

	DBGLOG(SCN, INFO, "nicCmdEventSetStopSchedScan OID done, release lock and send event to uplayer\n");
	/*Due to dead lock issue, need to release the IO control before calling kernel APIs */
	kalSchedScanStopped(prAdapter->prGlueInfo, !prCmdInfo->fgIsOid);

}

/* Statistics responder */
void nicCmdEventQueryXmitOk(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4Data;
	uint64_t *pu8Data;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		if (prCmdInfo->u4InformationBufferLength == sizeof(uint32_t)) {
			u4QueryInfoLen = sizeof(uint32_t);

			pu4Data = (uint32_t *) prCmdInfo->pvInformationBuffer;
			*pu4Data = (uint32_t) prEventStatistics->rTransmittedFragmentCount.QuadPart;
		} else {
			u4QueryInfoLen = sizeof(uint64_t);

			pu8Data = (uint64_t *) prCmdInfo->pvInformationBuffer;
			*pu8Data = prEventStatistics->rTransmittedFragmentCount.QuadPart;
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryRecvOk(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4Data;
	uint64_t *pu8Data;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		if (prCmdInfo->u4InformationBufferLength == sizeof(uint32_t)) {
			u4QueryInfoLen = sizeof(uint32_t);

			pu4Data = (uint32_t *) prCmdInfo->pvInformationBuffer;
			*pu4Data = (uint32_t) prEventStatistics->rReceivedFragmentCount.QuadPart;
		} else {
			u4QueryInfoLen = sizeof(uint64_t);

			pu8Data = (uint64_t *) prCmdInfo->pvInformationBuffer;
			*pu8Data = prEventStatistics->rReceivedFragmentCount.QuadPart;
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryXmitError(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4Data;
	uint64_t *pu8Data;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		if (prCmdInfo->u4InformationBufferLength == sizeof(uint32_t)) {
			u4QueryInfoLen = sizeof(uint32_t);

			pu4Data = (uint32_t *) prCmdInfo->pvInformationBuffer;
			*pu4Data = (uint32_t) prEventStatistics->rFailedCount.QuadPart;
		} else {
			u4QueryInfoLen = sizeof(uint64_t);

			pu8Data = (uint64_t *) prCmdInfo->pvInformationBuffer;
			*pu8Data = (uint64_t) prEventStatistics->rFailedCount.QuadPart;
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryRecvError(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4Data;
	uint64_t *pu8Data;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		if (prCmdInfo->u4InformationBufferLength == sizeof(uint32_t)) {
			u4QueryInfoLen = sizeof(uint32_t);

			pu4Data = (uint32_t *) prCmdInfo->pvInformationBuffer;
			*pu4Data = (uint32_t) prEventStatistics->rFCSErrorCount.QuadPart;
			/* @FIXME, RX_ERROR_DROP_COUNT/RX_FIFO_FULL_DROP_COUNT is not calculated */
		} else {
			u4QueryInfoLen = sizeof(uint64_t);

			pu8Data = (uint64_t *) prCmdInfo->pvInformationBuffer;
			*pu8Data = prEventStatistics->rFCSErrorCount.QuadPart;
			/* @FIXME, RX_ERROR_DROP_COUNT/RX_FIFO_FULL_DROP_COUNT is not calculated */
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryRecvNoBuffer(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4Data;
	uint64_t *pu8Data;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		if (prCmdInfo->u4InformationBufferLength == sizeof(uint32_t)) {
			u4QueryInfoLen = sizeof(uint32_t);

			pu4Data = (uint32_t *) prCmdInfo->pvInformationBuffer;
			*pu4Data = 0;	/* @FIXME? */
		} else {
			u4QueryInfoLen = sizeof(uint64_t);

			pu8Data = (uint64_t *) prCmdInfo->pvInformationBuffer;
			*pu8Data = 0;	/* @FIXME? */
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryRecvCrcError(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4Data;
	uint64_t *pu8Data;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		if (prCmdInfo->u4InformationBufferLength == sizeof(uint32_t)) {
			u4QueryInfoLen = sizeof(uint32_t);

			pu4Data = (uint32_t *) prCmdInfo->pvInformationBuffer;
			*pu4Data = (uint32_t) prEventStatistics->rFCSErrorCount.QuadPart;
		} else {
			u4QueryInfoLen = sizeof(uint64_t);

			pu8Data = (uint64_t *) prCmdInfo->pvInformationBuffer;
			*pu8Data = prEventStatistics->rFCSErrorCount.QuadPart;
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryRecvErrorAlignment(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4Data;
	uint64_t *pu8Data;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		if (prCmdInfo->u4InformationBufferLength == sizeof(uint32_t)) {
			u4QueryInfoLen = sizeof(uint32_t);

			pu4Data = (uint32_t *) prCmdInfo->pvInformationBuffer;
			*pu4Data = (uint32_t) 0;	/* @FIXME */
		} else {
			u4QueryInfoLen = sizeof(uint64_t);

			pu8Data = (uint64_t *) prCmdInfo->pvInformationBuffer;
			*pu8Data = 0;	/* @FIXME */
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryXmitOneCollision(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4Data;
	uint64_t *pu8Data;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		if (prCmdInfo->u4InformationBufferLength == sizeof(uint32_t)) {
			u4QueryInfoLen = sizeof(uint32_t);

			pu4Data = (uint32_t *) prCmdInfo->pvInformationBuffer;
			*pu4Data =
			    (uint32_t) (prEventStatistics->rMultipleRetryCount.QuadPart -
				       prEventStatistics->rRetryCount.QuadPart);
		} else {
			u4QueryInfoLen = sizeof(uint64_t);

			pu8Data = (uint64_t *) prCmdInfo->pvInformationBuffer;
			*pu8Data =
			    (uint64_t) (prEventStatistics->rMultipleRetryCount.QuadPart -
				       prEventStatistics->rRetryCount.QuadPart);
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryXmitMoreCollisions(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4Data;
	uint64_t *pu8Data;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		if (prCmdInfo->u4InformationBufferLength == sizeof(uint32_t)) {
			u4QueryInfoLen = sizeof(uint32_t);

			pu4Data = (uint32_t *) prCmdInfo->pvInformationBuffer;
			*pu4Data = (uint32_t) prEventStatistics->rMultipleRetryCount.QuadPart;
		} else {
			u4QueryInfoLen = sizeof(uint64_t);

			pu8Data = (uint64_t *) prCmdInfo->pvInformationBuffer;
			*pu8Data = (uint64_t) prEventStatistics->rMultipleRetryCount.QuadPart;
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicCmdEventQueryXmitMaxCollisions(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EVENT_STATISTICS *prEventStatistics;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;
	uint32_t *pu4Data;
	uint64_t *pu8Data;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventStatistics = (struct EVENT_STATISTICS *) pucEventBuf;

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		if (prCmdInfo->u4InformationBufferLength == sizeof(uint32_t)) {
			u4QueryInfoLen = sizeof(uint32_t);

			pu4Data = (uint32_t *) prCmdInfo->pvInformationBuffer;
			*pu4Data = (uint32_t) prEventStatistics->rFailedCount.QuadPart;
		} else {
			u4QueryInfoLen = sizeof(uint64_t);

			pu8Data = (uint64_t *) prCmdInfo->pvInformationBuffer;
			*pu8Data = (uint64_t) prEventStatistics->rFailedCount.QuadPart;
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when command by OID/ioctl has been timeout
*
* @param prAdapter          Pointer to the Adapter structure.
* @param prCmdInfo          Pointer to the command information
*
* @return TRUE
*         FALSE
*/
/*----------------------------------------------------------------------------*/
void nicOidCmdTimeoutCommon(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo)
{
	ASSERT(prAdapter);

	if (prCmdInfo->fgIsOid)
		kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_FAILURE);
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is a generic command timeout handler
*
* @param pfnOidHandler      Pointer to the OID handler
*
* @return none
*/
/*----------------------------------------------------------------------------*/
void nicCmdTimeoutCommon(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo)
{
	ASSERT(prAdapter);
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when command for entering RF test has
*        failed sending due to timeout (highly possibly by firmware crash)
*
* @param prAdapter          Pointer to the Adapter structure.
* @param prCmdInfo          Pointer to the command information
*
* @return none
*
*/
/*----------------------------------------------------------------------------*/
void nicOidCmdEnterRFTestTimeout(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo)
{
	ASSERT(prAdapter);

	/* 1. Remove pending TX frames */
	nicTxRelease(prAdapter, TRUE);

	/* 1.1 clear pending Security / Management Frames */
	kalClearSecurityFrames(prAdapter->prGlueInfo);
	kalClearMgmtFrames(prAdapter->prGlueInfo);

	/* 1.2 clear pending TX packet queued in glue layer */
	kalFlushPendingTxPackets(prAdapter->prGlueInfo);

	/* 2. indicate for OID failure */
	kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_FAILURE);
}

#if CFG_SUPPORT_QA_TOOL
/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when received dump memory event packet.
*        transfer the memory data to the IQ format data and write into file
*
* @param prIQAry     Pointer to the array store I or Q data.
*		 prDataLen   The return data length - bytes
*        u4IQ        0: get I data
*                    1 : get Q data
*
* @return -1: open file error
*
*/
/*----------------------------------------------------------------------------*/
int32_t GetIQData(struct ADAPTER *prAdapter, int32_t **prIQAry, uint32_t *prDataLen, uint32_t u4IQ, uint32_t u4GetWf1)
{
	uint8_t aucPath[50];	/* the path for iq data dump out */
	uint8_t aucData[50];	/* iq data in string format */
	uint32_t i = 0, j = 0, count = 0;
	int32_t ret = -1;
	int32_t rv;
	struct file *file = NULL;

	*prIQAry = prAdapter->rIcapInfo.au4IQData;

	/* sprintf(aucPath, "/pattern.txt");             // CSD's Pattern */
	sprintf(aucPath, "/dump_out_%05hu_WF%u.txt", prAdapter->rIcapInfo.u2DumpIndex - 1, u4GetWf1);
	DBGLOG(INIT, INFO, "iCap Read Dump File dump_out_%05hu_WF%u.txt\n",
	       prAdapter->rIcapInfo.u2DumpIndex - 1, u4GetWf1);

	file = kalFileOpen(aucPath, O_RDONLY, 0);

	if ((file != NULL) && !IS_ERR(file)) {
		/* read 1K data per time */
		for (i = 0; i < RTN_IQ_DATA_LEN / sizeof(uint32_t);
		     i++, prAdapter->rIcapInfo.au4Offset[u4GetWf1][u4IQ] += IQ_FILE_LINE_OFFSET) {
			if (kalFileRead(file, prAdapter->rIcapInfo.au4Offset[u4GetWf1][u4IQ],
					aucData, IQ_FILE_IQ_STR_LEN) == 0)
				break;

			count = 0;

			for (j = 0; j < 8; j++) {
				if (aucData[j] != ' ')
					aucData[count++] = aucData[j];
			}

			aucData[count] = '\0';

			/* transfer data format (string to int) */
			rv = kstrtoint(aucData, 0, &prAdapter->rIcapInfo.au4IQData[i]);
		}
		*prDataLen = i * sizeof(uint32_t);
		kalFileClose(file);
		ret = 0;
	}

	DBGLOG(INIT, INFO, "MT6632 : QA_AGENT GetIQData prDataLen = %d\n", *prDataLen);
	DBGLOG(INIT, INFO, "MT6632 : QA_AGENT GetIQData i = %d\n", i);

	return ret;
}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when received dump memory event packet.
*        transfer the memory data to the IQ format data and write into file
*
* @param prEventDumpMem     Pointer to the event dump memory structure.
*
* @return 0: SUCCESS, -1: FAIL
*
*/
/*----------------------------------------------------------------------------*/

uint32_t nicTsfRawData2IqFmt(struct EVENT_DUMP_MEM *prEventDumpMem, struct ICAP_INFO_T *prIcap)
{
	static uint8_t aucPathWF0[40];	/* the path for iq data dump out */
	static uint8_t aucPathWF1[40];	/* the path for iq data dump out */
	static uint8_t aucPathRAWWF0[40];	/* the path for iq data dump out */
	static uint8_t aucPathRAWWF1[40];	/* the path for iq data dump out */
	uint8_t *pucDataWF0 = NULL;	/* the data write into file */
	uint8_t *pucDataWF1 = NULL;	/* the data write into file */
	uint8_t *pucDataRAWWF0 = NULL;	/* the data write into file */
	uint8_t *pucDataRAWWF1 = NULL;	/* the data write into file */
	uint32_t u4SrcOffset;	/* record the buffer offset */
	uint32_t u4FmtLen = 0;	/* bus format length */
	uint32_t u4CpyLen = 0;
	uint32_t u4RemainByte;
	uint32_t u4DataWBufSize = 150;
	uint32_t u4DataRAWWBufSize = 150;
	uint32_t u4DataWLenF0 = 0;
	uint32_t u4DataWLenF1 = 0;
	uint32_t u4DataRAWWLenF0 = 0;
	uint32_t u4DataRAWWLenF1 = 0;

	u_int8_t fgAppend;
	int32_t u4Iqc160WF0Q0, u4Iqc160WF1I1;

	static uint8_t ucDstOffset;	/* for alignment. bcs we send 2KB data per packet,*/
								/*the data will not align in 12 bytes case. */
	static uint32_t u4CurTimeTick;

	static union ICAP_BUS_FMT icapBusData;
	uint32_t *ptr;

	pucDataWF0 = kmalloc(u4DataWBufSize, GFP_KERNEL);
	pucDataWF1 = kmalloc(u4DataWBufSize, GFP_KERNEL);
	pucDataRAWWF0 = kmalloc(u4DataRAWWBufSize, GFP_KERNEL);
	pucDataRAWWF1 = kmalloc(u4DataRAWWBufSize, GFP_KERNEL);


	if ((!pucDataWF0) || (!pucDataWF1) || (!pucDataRAWWF0) || (!pucDataRAWWF1)) {
		DBGLOG(INIT, ERROR, "kmalloc failed.\n");
		kfree(pucDataWF0);
		kfree(pucDataWF1);
		kfree(pucDataRAWWF0);
		kfree(pucDataRAWWF1);
		ASSERT(-1);
		return -1;
	}

	fgAppend = TRUE;
	if (prEventDumpMem->ucFragNum == 1) {

		u4CurTimeTick = kalGetTimeTick();
		/* Store memory dump into sdcard,
		 * path /sdcard/dump_<current  system tick>_<memory address>_<memory length>.hex
		 */
#if defined(LINUX)

		/*if blbist mkdir undre /data/blbist, the dump files wouls put on it */
		scnprintf(aucPathWF0, sizeof(aucPathWF0), "/tmp/dump_out_%05hu_WF0.txt", prIcap->u2DumpIndex);
		scnprintf(aucPathWF1, sizeof(aucPathWF1), "/tmp/dump_out_%05hu_WF1.txt", prIcap->u2DumpIndex);
		if (kalCheckPath(aucPathWF0) == -1) {
			kalMemSet(aucPathWF0, 0x00, sizeof(aucPathWF0));
			scnprintf(aucPathWF0, sizeof(aucPathWF0),
				  "/data/dump_out_%05hu_WF0.txt", prIcap->u2DumpIndex);
		} else
			kalTrunkPath(aucPathWF0);

		if (kalCheckPath(aucPathWF1) == -1) {
			kalMemSet(aucPathWF1, 0x00, sizeof(aucPathWF1));
			scnprintf(aucPathWF1, sizeof(aucPathWF1),
				  "/data/dump_out_%05hu_WF1.txt", prIcap->u2DumpIndex);
		} else
			kalTrunkPath(aucPathWF1);

		scnprintf(aucPathRAWWF0, sizeof(aucPathRAWWF0),
			  "/dump_RAW_%hu_WF0.txt", prIcap->u2DumpIndex);
		scnprintf(aucPathRAWWF1, sizeof(aucPathRAWWF1),
			  "/dump_RAW_%hu_WF1.txt", prIcap->u2DumpIndex);
		if (kalCheckPath(aucPathRAWWF0) == -1) {
			kalMemSet(aucPathRAWWF0, 0x00, sizeof(aucPathRAWWF0));
			scnprintf(aucPathRAWWF0, sizeof(aucPathRAWWF0),
				  "/data/dump_RAW_%05hu_WF0.txt", prIcap->u2DumpIndex);
		} else
			kalTrunkPath(aucPathRAWWF0);

		if (kalCheckPath(aucPathRAWWF1) == -1) {
			kalMemSet(aucPathRAWWF1, 0x00, sizeof(aucPathRAWWF1));
			scnprintf(aucPathRAWWF1, sizeof(aucPathRAWWF1),
				  "/data/dump_RAW_%05hu_WF1.txt", prIcap->u2DumpIndex);
		} else
			kalTrunkPath(aucPathRAWWF1);

#else
		kal_sprintf_ddk(aucPathWF0, sizeof(aucPathWF0),
				u4CurTimeTick, prEventDumpMem->u4Address,
				prEventDumpMem->u4Length + prEventDumpMem->u4RemainLength);
		kal_sprintf_ddk(aucPathWF1, sizeof(aucPathWF1),
				u4CurTimeTick, prEventDumpMem->u4Address,
				prEventDumpMem->u4Length + prEventDumpMem->u4RemainLength);
#endif
		/* fgAppend = FALSE; */
	}

	ptr = (uint32_t *)(&prEventDumpMem->aucBuffer[0]);
	/*DBGLOG(INIT, INFO, ": ==> (prEventDumpMem = %08x %08x %08x)\n", *(ptr), *(ptr + 4), *(ptr + 8));*/
	/*DBGLOG(INIT, INFO, ": ==> (prEventDumpMem->eIcapContent = %x)\n", prEventDumpMem->eIcapContent);*/

	for (u4SrcOffset = 0, u4RemainByte = prEventDumpMem->u4Length; u4RemainByte > 0;) {
		u4FmtLen =
(prEventDumpMem->eIcapContent == ICAP_CONTENT_SPECTRUM) ? sizeof(struct SPECTRUM_BUS_FMT) : sizeof(union ICAP_BUS_FMT);
		/* 4 bytes : 12 bytes */
		u4CpyLen = (u4RemainByte - u4FmtLen >= 0) ? u4FmtLen : u4RemainByte;

		if ((ucDstOffset + u4CpyLen) > sizeof(icapBusData)) {
			DBGLOG(INIT, ERROR,
			       "ucDstOffset(%u) + u4CpyLen(%u) exceed bound of icapBusData\n",
			       ucDstOffset, u4CpyLen);
			kfree(pucDataWF0);
			kfree(pucDataWF1);
			kfree(pucDataRAWWF0);
			kfree(pucDataRAWWF1);
			ASSERT(-1);
			return -1;
		}
		memcpy((uint8_t *)&icapBusData + ucDstOffset, &prEventDumpMem->aucBuffer[0] + u4SrcOffset, u4CpyLen);
#if 0
		if (prEventDumpMem->eIcapContent == ICAP_CONTENT_ADC) {
			sprintf(aucDataWF0, "%8d,%8d\n", icapBusData.rAdcBusData.u4Dcoc0I,
				icapBusData.rAdcBusData.u4Dcoc0Q);
			sprintf(aucDataWF1, "%8d,%8d\n", icapBusData.rAdcBusData.u4Dcoc1I,
				icapBusData.rAdcBusData.u4Dcoc1Q);
		}
#endif
		if (prEventDumpMem->eIcapContent == ICAP_CONTENT_FIIQ ||
		    prEventDumpMem->eIcapContent == ICAP_CONTENT_FDIQ) {
			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize, "%8d,%8d\n",
						 icapBusData.rIqcBusData.u4Iqc0I,
						 icapBusData.rIqcBusData.u4Iqc0Q);
			u4DataWLenF1 = scnprintf(pucDataWF1, u4DataWBufSize, "%8d,%8d\n",
						 icapBusData.rIqcBusData.u4Iqc1I,
						 icapBusData.rIqcBusData.u4Iqc1Q);
		} else if (prEventDumpMem->eIcapContent - 1000 == ICAP_CONTENT_FIIQ
			   || prEventDumpMem->eIcapContent - 1000 == ICAP_CONTENT_FDIQ) {
			u4Iqc160WF0Q0 =
			    icapBusData.rIqc160BusData.u4Iqc0Q0P1 | (icapBusData.rIqc160BusData.u4Iqc0Q0P2 << 8);
			u4Iqc160WF1I1 =
			    icapBusData.rIqc160BusData.u4Iqc1I1P1 | (icapBusData.rIqc160BusData.u4Iqc1I1P2 << 4);

			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize, "%8d,%8d\n%8d,%8d\n",
						 icapBusData.rIqc160BusData.u4Iqc0I0, u4Iqc160WF0Q0,
						 icapBusData.rIqc160BusData.u4Iqc0I1,
						 icapBusData.rIqc160BusData.u4Iqc0Q1);

			u4DataWLenF1 = scnprintf(pucDataWF1, u4DataWBufSize, "%8d,%8d\n%8d,%8d\n",
						 icapBusData.rIqc160BusData.u4Iqc1I0,
						 icapBusData.rIqc160BusData.u4Iqc1Q0, u4Iqc160WF1I1,
						 icapBusData.rIqc160BusData.u4Iqc1Q1);

		} else if (prEventDumpMem->eIcapContent == ICAP_CONTENT_SPECTRUM) {
			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize, "%8d,%8d\n",
						 icapBusData.rSpectrumBusData.u4DcocI,
						 icapBusData.rSpectrumBusData.u4DcocQ);
		} else if (prEventDumpMem->eIcapContent == ICAP_CONTENT_ADC) {
			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize,
						 "%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n",
						 icapBusData.rPackedAdcBusData.u4AdcI0T0,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T0,
						 icapBusData.rPackedAdcBusData.u4AdcI0T1,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T1,
						 icapBusData.rPackedAdcBusData.u4AdcI0T2,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T2,
						 icapBusData.rPackedAdcBusData.u4AdcI0T3,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T3,
						 icapBusData.rPackedAdcBusData.u4AdcI0T4,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T4,
						 icapBusData.rPackedAdcBusData.u4AdcI0T5,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T5);

			u4DataWLenF1 = scnprintf(pucDataWF1, u4DataWBufSize,
						 "%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n",
						 icapBusData.rPackedAdcBusData.u4AdcI1T0,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T0,
						 icapBusData.rPackedAdcBusData.u4AdcI1T1,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T1,
						 icapBusData.rPackedAdcBusData.u4AdcI1T2,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T2,
						 icapBusData.rPackedAdcBusData.u4AdcI1T3,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T3,
						 icapBusData.rPackedAdcBusData.u4AdcI1T4,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T4,
						 icapBusData.rPackedAdcBusData.u4AdcI1T5,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T5);
		} else if (prEventDumpMem->eIcapContent - 2000 == ICAP_CONTENT_ADC) {
			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize, "%8d,%8d\n%8d,%8d\n%8d,%8d\n",
						 icapBusData.rPackedAdcBusData.u4AdcI0T0,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T0,
						 icapBusData.rPackedAdcBusData.u4AdcI0T1,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T1,
						 icapBusData.rPackedAdcBusData.u4AdcI0T2,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T2);

			u4DataWLenF1 = scnprintf(pucDataWF1, u4DataWBufSize, "%8d,%8d\n%8d,%8d\n%8d,%8d\n",
						 icapBusData.rPackedAdcBusData.u4AdcI1T0,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T0,
						 icapBusData.rPackedAdcBusData.u4AdcI1T1,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T1,
						 icapBusData.rPackedAdcBusData.u4AdcI1T2,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T2);
		} else if (prEventDumpMem->eIcapContent == ICAP_CONTENT_TOAE) {
			/* actually, this is DCOC. we take TOAE as DCOC */
			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize, "%8d,%8d\n",
						 icapBusData.rAdcBusData.u4Dcoc0I, icapBusData.rAdcBusData.u4Dcoc0Q);
			u4DataWLenF1 = scnprintf(pucDataWF1, u4DataWBufSize, "%8d,%8d\n",
						 icapBusData.rAdcBusData.u4Dcoc1I, icapBusData.rAdcBusData.u4Dcoc1Q);
		}
		if (u4CpyLen == u4FmtLen) {	/* the data format is complete */
			kalWriteToFile(aucPathWF0, fgAppend, pucDataWF0, u4DataWLenF0);
			kalWriteToFile(aucPathWF1, fgAppend, pucDataWF1, u4DataWLenF1);
		}
		ptr = (uint32_t *)(&prEventDumpMem->aucBuffer[0] + u4SrcOffset);
		u4DataRAWWLenF0 = scnprintf(pucDataRAWWF0, u4DataWBufSize, "%08x%08x%08x\n",
					    *(ptr + 2), *(ptr + 1), *ptr);
		kalWriteToFile(aucPathRAWWF0, fgAppend, pucDataRAWWF0, u4DataRAWWLenF0);
		kalWriteToFile(aucPathRAWWF1, fgAppend, pucDataRAWWF1, u4DataRAWWLenF1);

		u4RemainByte -= u4CpyLen;
		u4SrcOffset += u4CpyLen;	/* shift offset */
		ucDstOffset = 0;	/* only use ucDstOffset at first packet for align 2KB */
	}
	/* if this is a last packet, we can't transfer the remain data.
	 *  bcs we can't guarantee the data is complete align data format
	 */
	if (u4CpyLen != u4FmtLen) {	/* the data format is complete */
		ucDstOffset = u4CpyLen;	/* not align 2KB, keep the data and next packet data will append it */
	}

	kfree(pucDataWF0);
	kfree(pucDataWF1);
	kfree(pucDataRAWWF0);
	kfree(pucDataRAWWF1);

	if (u4RemainByte < 0) {
		ASSERT(-1);
		return -1;
	}

	return 0;
}
#endif /* CFG_SUPPORT_QA_TOOL */

#if CFG_SUPPORT_CAL_RESULT_BACKUP_TO_HOST
void nicCmdEventQueryCalBackupV2(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct PARAM_CAL_BACKUP_STRUCT_V2 *prCalBackupDataV2Info;
	struct CMD_CAL_BACKUP_STRUCT_V2 *prEventCalBackupDataV2;
	uint32_t u4QueryInfoLen, u4QueryInfo, u4TempAddress;
	struct GLUE_INFO *prGlueInfo;

	DBGLOG(RFTEST, INFO, "%s\n", __func__);

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);


	prGlueInfo = prAdapter->prGlueInfo;
	prEventCalBackupDataV2 = (struct CMD_CAL_BACKUP_STRUCT_V2 *) (pucEventBuf);

	u4QueryInfoLen = sizeof(struct CMD_CAL_BACKUP_STRUCT_V2);

	prCalBackupDataV2Info = (struct PARAM_CAL_BACKUP_STRUCT_V2 *) prCmdInfo->pvInformationBuffer;
#if 0
	DBGLOG(RFTEST, INFO, "============ Receive a Cal Data EVENT (Info) ============\n");
	DBGLOG(RFTEST, INFO, "Reason = %d\n", prEventCalBackupDataV2->ucReason);
	DBGLOG(RFTEST, INFO, "Action = %d\n", prEventCalBackupDataV2->ucAction);
	DBGLOG(RFTEST, INFO, "NeedResp = %d\n", prEventCalBackupDataV2->ucNeedResp);
	DBGLOG(RFTEST, INFO, "FragNum = %d\n", prEventCalBackupDataV2->ucFragNum);
	DBGLOG(RFTEST, INFO, "RomRam = %d\n", prEventCalBackupDataV2->ucRomRam);
	DBGLOG(RFTEST, INFO, "ThermalValue = %d\n", prEventCalBackupDataV2->u4ThermalValue);
	DBGLOG(RFTEST, INFO, "Address = 0x%08x\n", prEventCalBackupDataV2->u4Address);
	DBGLOG(RFTEST, INFO, "Length = %d\n", prEventCalBackupDataV2->u4Length);
	DBGLOG(RFTEST, INFO, "RemainLength = %d\n", prEventCalBackupDataV2->u4RemainLength);
	DBGLOG(RFTEST, INFO, "=========================================================\n");
#endif

	if (prEventCalBackupDataV2->ucReason == 0 && prEventCalBackupDataV2->ucAction == 0) {
		DBGLOG(RFTEST, INFO, "Received an EVENT for Query Thermal Temp.\n");
		prCalBackupDataV2Info->u4ThermalValue = prEventCalBackupDataV2->u4ThermalValue;
		g_rBackupCalDataAllV2.u4ThermalInfo = prEventCalBackupDataV2->u4ThermalValue;
		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	} else if (prEventCalBackupDataV2->ucReason == 0 && prEventCalBackupDataV2->ucAction == 1) {
		DBGLOG(RFTEST, INFO, "Received an EVENT for Query Total Cal Data Length.\n");
		prCalBackupDataV2Info->u4Length = prEventCalBackupDataV2->u4Length;

		if (prEventCalBackupDataV2->ucRomRam == 0)
			g_rBackupCalDataAllV2.u4ValidRomCalDataLength = prEventCalBackupDataV2->u4Length;
		else if (prEventCalBackupDataV2->ucRomRam == 1)
			g_rBackupCalDataAllV2.u4ValidRamCalDataLength = prEventCalBackupDataV2->u4Length;

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	} else if (prEventCalBackupDataV2->ucReason == 2 && prEventCalBackupDataV2->ucAction == 4) {
		DBGLOG(RFTEST, INFO, "Received an EVENT for Query All Cal (%s) Data. FragNum = %d\n",
			prCalBackupDataV2Info->ucRomRam == 0 ? "ROM" : "RAM",
			prEventCalBackupDataV2->ucFragNum);
		prCalBackupDataV2Info->u4Address = prEventCalBackupDataV2->u4Address;
		prCalBackupDataV2Info->u4Length = prEventCalBackupDataV2->u4Length;
		prCalBackupDataV2Info->u4RemainLength = prEventCalBackupDataV2->u4RemainLength;
		prCalBackupDataV2Info->ucFragNum = prEventCalBackupDataV2->ucFragNum;

		/* Copy Cal Data From FW to Driver Array */
		if (prEventCalBackupDataV2->ucRomRam == 0) {
			u4TempAddress = prEventCalBackupDataV2->u4Address;
			kalMemCopy((uint8_t *)(g_rBackupCalDataAllV2.au4RomCalData) + u4TempAddress,
			(uint8_t *)(prEventCalBackupDataV2->au4Buffer),
			prEventCalBackupDataV2->u4Length);
		} else if (prEventCalBackupDataV2->ucRomRam == 1) {
			u4TempAddress = prEventCalBackupDataV2->u4Address;
			kalMemCopy((uint8_t *)(g_rBackupCalDataAllV2.au4RamCalData) + u4TempAddress,
			(uint8_t *)(prEventCalBackupDataV2->au4Buffer),
			prEventCalBackupDataV2->u4Length);
		}

		if (prEventCalBackupDataV2->u4Address == 0xFFFFFFFF) {
			DBGLOG(RFTEST, INFO, "RLM CMD : Address Error!!!!!!!!!!!\n");
		} else if (prEventCalBackupDataV2->u4RemainLength == 0
					&& prEventCalBackupDataV2->ucRomRam == 1) {
			DBGLOG(RFTEST, INFO, "RLM CMD : Get Cal Data from FW (%s). Finish!!!!!!!!!!!\n",
				prCalBackupDataV2Info->ucRomRam == 0 ? "ROM" : "RAM");
		} else if (prEventCalBackupDataV2->u4RemainLength == 0
					&& prEventCalBackupDataV2->ucRomRam == 0) {
			DBGLOG(RFTEST, INFO, "RLM CMD : Get Cal Data from FW (%s). Finish!!!!!!!!!!!\n",
				prCalBackupDataV2Info->ucRomRam == 0 ? "ROM" : "RAM");
			prCalBackupDataV2Info->ucFragNum = 0;
			prCalBackupDataV2Info->ucRomRam = 1;
			prCalBackupDataV2Info->u4ThermalValue = 0;
			prCalBackupDataV2Info->u4Address = 0;
			prCalBackupDataV2Info->u4Length = 0;
			prCalBackupDataV2Info->u4RemainLength = 0;
			DBGLOG(RFTEST, INFO, "RLM CMD : Get Cal Data from FW (%s). Start!!!!!!!!!!!!!!!!\n",
				prCalBackupDataV2Info->ucRomRam == 0 ? "ROM" : "RAM");
			DBGLOG(RFTEST, INFO, "Thermal Temp = %d\n", g_rBackupCalDataAllV2.u4ThermalInfo);
			wlanoidQueryCalBackupV2(prAdapter,
				prCalBackupDataV2Info,
				sizeof(struct PARAM_CAL_BACKUP_STRUCT_V2),
				&u4QueryInfo);
		} else {
			wlanoidSendCalBackupV2Cmd(prAdapter,
				   prCmdInfo->pvInformationBuffer, prCmdInfo->u4InformationBufferLength);
		}
	} else if (prEventCalBackupDataV2->ucReason == 3 && prEventCalBackupDataV2->ucAction == 5) {
		DBGLOG(RFTEST, INFO,
			"Received an EVENT for Send All Cal Data. FragNum = %d\n",
			prEventCalBackupDataV2->ucFragNum);
		prCalBackupDataV2Info->u4Address = prEventCalBackupDataV2->u4Address;
		prCalBackupDataV2Info->u4Length = prEventCalBackupDataV2->u4Length;
		prCalBackupDataV2Info->u4RemainLength = prEventCalBackupDataV2->u4RemainLength;
		prCalBackupDataV2Info->ucFragNum = prEventCalBackupDataV2->ucFragNum;

		if (prEventCalBackupDataV2->u4RemainLength == 0
			|| prEventCalBackupDataV2->u4Address == 0xFFFFFFFF) {
			kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
		} else {
			wlanoidSendCalBackupV2Cmd(prAdapter,
				   prCmdInfo->pvInformationBuffer, prCmdInfo->u4InformationBufferLength);
		}
	} else {
		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

}
#endif

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called to handle dump burst event
*
* @param prAdapter          Pointer to the Adapter structure.
* @param prCmdInfo         Pointer to the command information
* @param pucEventBuf       Pointer to event buffer
*
* @return none
*
*/
/*----------------------------------------------------------------------------*/
void nicEventQueryMemDump(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct EVENT_DUMP_MEM *prEventDumpMem;
	static uint8_t aucPath[256];
	static uint8_t aucPath_done[300];
	static uint32_t u4CurTimeTick;

	ASSERT(prAdapter);
	ASSERT(pucEventBuf);

	sprintf(aucPath, "/dump_%05hu.hex", prAdapter->rIcapInfo.u2DumpIndex);

	prEventDumpMem = (struct EVENT_DUMP_MEM *) (pucEventBuf);

	if (kalCheckPath(aucPath) == -1) {
		kalMemSet(aucPath, 0x00, 256);
		sprintf(aucPath, "/data/dump_%05hu.hex", prAdapter->rIcapInfo.u2DumpIndex);
	}

	if (prEventDumpMem->ucFragNum == 1) {
		/* Store memory dump into sdcard,
		 * path /sdcard/dump_<current  system tick>_<memory address>_<memory length>.hex
		 */
		u4CurTimeTick = kalGetTimeTick();
#if defined(LINUX)

		/*if blbist mkdir undre /data/blbist, the dump files wouls put on it */
		sprintf(aucPath, "/dump_%05hu.hex", prAdapter->rIcapInfo.u2DumpIndex);
		if (kalCheckPath(aucPath) == -1) {
			kalMemSet(aucPath, 0x00, 256);
			sprintf(aucPath, "/data/dump_%05hu.hex", prAdapter->rIcapInfo.u2DumpIndex);
		}
#else
		kal_sprintf_ddk(aucPath, sizeof(aucPath),
				u4CurTimeTick,
				prEventDumpMem->u4Address, prEventDumpMem->u4Length + prEventDumpMem->u4RemainLength);
#endif
		kalWriteToFile(aucPath, FALSE, &prEventDumpMem->aucBuffer[0], prEventDumpMem->u4Length);
	} else {
		/* Append current memory dump to the hex file */
		kalWriteToFile(aucPath, TRUE, &prEventDumpMem->aucBuffer[0], prEventDumpMem->u4Length);
	}
#if CFG_SUPPORT_QA_TOOL
	nicTsfRawData2IqFmt(prEventDumpMem, &prAdapter->rIcapInfo);
#endif /* CFG_SUPPORT_QA_TOOL */
	DBGLOG(INIT, INFO,
	       "iCap : ==> (u4RemainLength = %x, u4Address=%x )\n", prEventDumpMem->u4RemainLength,
	       prEventDumpMem->u4Address);

	if (prEventDumpMem->u4RemainLength == 0 || prEventDumpMem->u4Address == 0xFFFFFFFF) {

		/* The request is finished or firmware response a error */
		/* Reply time tick to iwpriv */

		prAdapter->rIcapInfo.fgIcapEnable = FALSE;
		prAdapter->rIcapInfo.fgCaptureDone = TRUE;

		sprintf(aucPath_done, "/file_dump_done.txt");
		if (kalCheckPath(aucPath_done) == -1) {
			kalMemSet(aucPath_done, 0x00, 256);
			sprintf(aucPath_done, "/data/file_dump_done.txt");
		}
		DBGLOG(INIT, INFO, ": ==> gen done_file\n");
		kalWriteToFile(aucPath_done, FALSE, aucPath_done, sizeof(aucPath_done));
#if CFG_SUPPORT_QA_TOOL
		prAdapter->rIcapInfo.au4Offset[0][0] = 0;
		prAdapter->rIcapInfo.au4Offset[0][1] = 9;
		prAdapter->rIcapInfo.au4Offset[1][0] = 0;
		prAdapter->rIcapInfo.au4Offset[1][1] = 9;
#endif /* CFG_SUPPORT_QA_TOOL */

		prAdapter->rIcapInfo.u2DumpIndex++;

	}

}

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when command for memory dump has
*        replied a event.
*
* @param prAdapter          Pointer to the Adapter structure.
* @param prCmdInfo         Pointer to the command information
* @param pucEventBuf       Pointer to event buffer
*
* @return none
*
*/
/*----------------------------------------------------------------------------*/
void nicCmdEventQueryMemDump(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct GLUE_INFO *prGlueInfo;
	struct EVENT_DUMP_MEM *prEventDumpMem;
	static uint8_t aucPath[256];
/*	static UINT_8 aucPath_done[300]; */
	static uint32_t u4CurTimeTick;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	/* 4 <2> Update information of OID */
	if (1) {
		prGlueInfo = prAdapter->prGlueInfo;
		prEventDumpMem = (struct EVENT_DUMP_MEM *) (pucEventBuf);

		u4QueryInfoLen = sizeof(struct PARAM_CUSTOM_MEM_DUMP_STRUCT *);

		if (prEventDumpMem->ucFragNum == 1) {
			/* Store memory dump into sdcard,
			 * path /sdcard/dump_<current  system tick>_<memory address>_<memory length>.hex
			 */
			u4CurTimeTick = kalGetTimeTick();
#if defined(LINUX)

			/* PeiHsuan add for avoiding out of memory 20160801 */
			if (prAdapter->rIcapInfo.u2DumpIndex >= 20)
				prAdapter->rIcapInfo.u2DumpIndex = 0;

			/*if blbist mkdir undre /data/blbist, the dump files wouls put on it */
			sprintf(aucPath, "/dump_%05hu.hex", prAdapter->rIcapInfo.u2DumpIndex);
			if (kalCheckPath(aucPath) == -1) {
				kalMemSet(aucPath, 0x00, 256);
				sprintf(aucPath, "/data/dump_%05hu.hex", prAdapter->rIcapInfo.u2DumpIndex);
			} else
				kalTrunkPath(aucPath);

			DBGLOG(INIT, INFO, "iCap Create New Dump File dump_%05hu.hex\n",
			       prAdapter->rIcapInfo.u2DumpIndex);
#else
			kal_sprintf_ddk(aucPath, sizeof(aucPath),
					u4CurTimeTick,
					prEventDumpMem->u4Address,
					prEventDumpMem->u4Length + prEventDumpMem->u4RemainLength);
			/* strcpy(aucPath, "dump.hex"); */
#endif
			kalWriteToFile(aucPath, FALSE, &prEventDumpMem->aucBuffer[0], prEventDumpMem->u4Length);
		} else {
			/* Append current memory dump to the hex file */
			kalWriteToFile(aucPath, TRUE, &prEventDumpMem->aucBuffer[0], prEventDumpMem->u4Length);
		}
#if CFG_SUPPORT_QA_TOOL
		nicTsfRawData2IqFmt(prEventDumpMem, &prAdapter->rIcapInfo);
#endif /* CFG_SUPPORT_QA_TOOL */
		if (prEventDumpMem->u4RemainLength == 0 || prEventDumpMem->u4Address == 0xFFFFFFFF) {
			/* The request is finished or firmware response a error */
			/* Reply time tick to iwpriv */
			if (prCmdInfo->fgIsOid) {

				/* the oid would be complete only in oid-trigger  mode,
				 * that is no need to if the event-trigger
				 */
				if (prAdapter->rIcapInfo.fgIcapEnable == FALSE) {
					*((uint32_t *) prCmdInfo->pvInformationBuffer) = u4CurTimeTick;
					kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery,
						       u4QueryInfoLen, WLAN_STATUS_SUCCESS);
				}
			}
			prAdapter->rIcapInfo.fgIcapEnable = FALSE;
			prAdapter->rIcapInfo.fgCaptureDone = TRUE;
#if defined(LINUX)

			prAdapter->rIcapInfo.u2DumpIndex++;

#else
			kal_sprintf_done_ddk(aucPath_done, sizeof(aucPath_done));
			kalWriteToFile(aucPath_done, FALSE, aucPath_done, sizeof(aucPath_done));
#endif
		} else {
#if defined(LINUX)

#else /* 2013/05/26 fw would try to send the buffer successfully */
			/* The memory dump request is not finished, Send next command */
			wlanSendMemDumpCmd(prAdapter,
					   prCmdInfo->pvInformationBuffer, prCmdInfo->u4InformationBufferLength);
#endif
		}
	}

	return;

}

#if CFG_SUPPORT_BATCH_SCAN
/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when event for SUPPORT_BATCH_SCAN
*
* @param prAdapter          Pointer to the Adapter structure.
* @param prCmdInfo          Pointer to the command information
* @param pucEventBuf        Pointer to the event buffer
*
* @return none
*
*/
/*----------------------------------------------------------------------------*/
void nicCmdEventBatchScanResult(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct EVENT_BATCH_RESULT *prEventBatchResult;
	struct GLUE_INFO *prGlueInfo;

	DBGLOG(SCN, TRACE, "nicCmdEventBatchScanResult");

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	/* 4 <2> Update information of OID */
	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prEventBatchResult = (struct EVENT_BATCH_RESULT *) pucEventBuf;

		u4QueryInfoLen = sizeof(struct EVENT_BATCH_RESULT);
		kalMemCopy(prCmdInfo->pvInformationBuffer, prEventBatchResult, sizeof(struct EVENT_BATCH_RESULT));

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

}
#endif

void nicEventHifCtrl(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
#if defined(_HIF_USB)
	struct EVENT_HIF_CTRL *prEventHifCtrl;

	prEventHifCtrl = (struct EVENT_HIF_CTRL *) (prEvent->aucBuffer);

	DBGLOG(HAL, INFO, "%s: EVENT_ID_HIF_CTRL\n", __func__);
	DBGLOG(HAL, INFO, "prEventHifCtrl->ucHifType = %hhu\n", prEventHifCtrl->ucHifType);
	DBGLOG(HAL, INFO, "prEventHifCtrl->ucHifTxTrafficStatus, prEventHifCtrl->ucHifRxTrafficStatus = %hhu, %hhu\n",
	       prEventHifCtrl->ucHifTxTrafficStatus, prEventHifCtrl->ucHifRxTrafficStatus);

	if (prEventHifCtrl->ucHifTxTrafficStatus == ENUM_HIF_TRAFFIC_IDLE &&
	    prEventHifCtrl->ucHifRxTrafficStatus == ENUM_HIF_TRAFFIC_IDLE) {	/* success */
		halUSBPreSuspendDone(prAdapter, NULL, prEvent->aucBuffer);
	} else if (prEventHifCtrl->ucHifTxTrafficStatus == ENUM_HIF_TRAFFIC_BUSY ||
		   prEventHifCtrl->ucHifRxTrafficStatus == ENUM_HIF_TRAFFIC_BUSY) {	/* busy */
		halUSBPreSuspendTimeout(prAdapter, NULL);
	} else if (prEventHifCtrl->ucHifTxTrafficStatus == ENUM_HIF_TRAFFIC_INVALID ||
		   prEventHifCtrl->ucHifRxTrafficStatus == ENUM_HIF_TRAFFIC_INVALID) {	/* invalid */
		halUSBPreSuspendTimeout(prAdapter, NULL);
	}
#endif
}

#if CFG_SUPPORT_BUILD_DATE_CODE
/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when event for build date code information
*        has been retrieved
*
* @param prAdapter          Pointer to the Adapter structure.
* @param prCmdInfo          Pointer to the command information
* @param pucEventBuf        Pointer to the event buffer
*
* @return none
*
*/
/*----------------------------------------------------------------------------*/
void nicCmdEventBuildDateCode(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct EVENT_BUILD_DATE_CODE *prEvent;
	struct GLUE_INFO *prGlueInfo;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	/* 4 <2> Update information of OID */
	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prEvent = (struct EVENT_BUILD_DATE_CODE *) pucEventBuf;

		u4QueryInfoLen = sizeof(uint8_t) * 16;
		kalMemCopy(prCmdInfo->pvInformationBuffer, prEvent->aucDateCode, sizeof(uint8_t) * 16);

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

}
#endif

/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when event for query STA link status
*        has been retrieved
*
* @param prAdapter          Pointer to the Adapter structure.
* @param prCmdInfo          Pointer to the command information
* @param pucEventBuf        Pointer to the event buffer
*
* @return none
*
*/
/*----------------------------------------------------------------------------*/
void nicCmdEventQueryStaStatistics(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct EVENT_STA_STATISTICS *prEvent;
	struct GLUE_INFO *prGlueInfo;
	struct PARAM_GET_STA_STATISTICS *prStaStatistics;
	enum ENUM_WMM_ACI eAci;
	struct STA_RECORD *prStaRec;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);
	ASSERT(prCmdInfo->pvInformationBuffer);

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prEvent = (struct EVENT_STA_STATISTICS *) pucEventBuf;
		prStaStatistics = (struct PARAM_GET_STA_STATISTICS *) prCmdInfo->pvInformationBuffer;

		u4QueryInfoLen = sizeof(struct PARAM_GET_STA_STATISTICS);

		/* Statistics from FW is valid */
		if (prEvent->u4Flags & BIT(0)) {
			prStaStatistics->ucPer = prEvent->ucPer;
			prStaStatistics->ucRcpi = prEvent->ucRcpi;
			prStaStatistics->u4PhyMode = prEvent->u4PhyMode;
			prStaStatistics->u2LinkSpeed = prEvent->u2LinkSpeed;

			prStaStatistics->u4TxFailCount = prEvent->u4TxFailCount;
			prStaStatistics->u4TxLifeTimeoutCount = prEvent->u4TxLifeTimeoutCount;
			prStaStatistics->u4TransmitCount = prEvent->u4TransmitCount;
			prStaStatistics->u4TransmitFailCount = prEvent->u4TransmitFailCount;
			prStaStatistics->u4Rate1TxCnt = prEvent->u4Rate1TxCnt;
			prStaStatistics->u4Rate1FailCnt = prEvent->u4Rate1FailCnt;

			prStaStatistics->ucTemperature = prEvent->ucTemperature;
			prStaStatistics->ucSkipAr = prEvent->ucSkipAr;
			prStaStatistics->ucArTableIdx = prEvent->ucArTableIdx;
			prStaStatistics->ucRateEntryIdx = prEvent->ucRateEntryIdx;
			prStaStatistics->ucRateEntryIdxPrev = prEvent->ucRateEntryIdxPrev;
			prStaStatistics->ucTxSgiDetectPassCnt = prEvent->ucTxSgiDetectPassCnt;
			prStaStatistics->ucAvePer = prEvent->ucAvePer;
#if (CFG_SUPPORT_RA_GEN == 0)
			kalMemCopy(prStaStatistics->aucArRatePer, prEvent->aucArRatePer,
				sizeof(prEvent->aucArRatePer));
			kalMemCopy(prStaStatistics->aucRateEntryIndex, prEvent->aucRateEntryIndex,
				sizeof(prEvent->aucRateEntryIndex));
#else
			prStaStatistics->u4AggRangeCtrl_0 = prEvent->u4AggRangeCtrl_0;
			prStaStatistics->u4AggRangeCtrl_1 = prEvent->u4AggRangeCtrl_1;
			prStaStatistics->ucRangeType = prEvent->ucRangeType;
			kalMemCopy(prStaStatistics->aucReserved5, prEvent->aucReserved5,
				sizeof(prEvent->aucReserved5));
#endif
			prStaStatistics->ucArStateCurr = prEvent->ucArStateCurr;
			prStaStatistics->ucArStatePrev = prEvent->ucArStatePrev;
			prStaStatistics->ucArActionType = prEvent->ucArActionType;
			prStaStatistics->ucHighestRateCnt = prEvent->ucHighestRateCnt;
			prStaStatistics->ucLowestRateCnt = prEvent->ucLowestRateCnt;
			prStaStatistics->u2TrainUp = prEvent->u2TrainUp;
			prStaStatistics->u2TrainDown = prEvent->u2TrainDown;
			kalMemCopy(&prStaStatistics->rTxVector, &prEvent->rTxVector,
				sizeof(prEvent->rTxVector));
			kalMemCopy(&prStaStatistics->rMibInfo, &prEvent->rMibInfo,
				sizeof(prEvent->rMibInfo));
			prStaStatistics->fgIsForceTxStream = prEvent->fgIsForceTxStream;
			prStaStatistics->fgIsForceSeOff = prEvent->fgIsForceSeOff;
#if (CFG_SUPPORT_RA_GEN == 0)
			kalMemCopy(prStaStatistics->aucReserved6, prEvent->aucReserved6,
				sizeof(prEvent->aucReserved6));
#else
			prStaStatistics->u2RaRunningCnt = prEvent->u2RaRunningCnt;
			prStaStatistics->ucRaStatus = prEvent->ucRaStatus;
			prStaStatistics->ucFlag = prEvent->ucFlag;
			kalMemCopy(&prStaStatistics->aucTxQuality, &prEvent->aucTxQuality,
				sizeof(prEvent->aucTxQuality));
			prStaStatistics->ucTxRateUpPenalty = prEvent->ucTxRateUpPenalty;
			prStaStatistics->ucLowTrafficMode = prEvent->ucLowTrafficMode;
			prStaStatistics->ucLowTrafficCount = prEvent->ucLowTrafficCount;
			prStaStatistics->ucLowTrafficDashBoard = prEvent->ucLowTrafficDashBoard;
			prStaStatistics->ucDynamicSGIState = prEvent->ucDynamicSGIState;
			prStaStatistics->ucDynamicSGIScore = prEvent->ucDynamicSGIScore;
			prStaStatistics->ucDynamicBWState = prEvent->ucDynamicBWState;
			prStaStatistics->ucDynamicGband256QAMState = prEvent->ucDynamicGband256QAMState;
			prStaStatistics->ucVhtNonSpRateState = prEvent->ucVhtNonSpRateState;
#endif
			prStaRec = cnmGetStaRecByIndex(prAdapter, prEvent->ucStaRecIdx);

			if (prStaRec) {
				/*link layer statistics */
				for (eAci = 0; eAci < WMM_AC_INDEX_NUM; eAci++) {
					prStaStatistics->arLinkStatistics[eAci].u4TxFailMsdu =
					    prEvent->arLinkStatistics[eAci].u4TxFailMsdu;
					prStaStatistics->arLinkStatistics[eAci].u4TxRetryMsdu =
					    prEvent->arLinkStatistics[eAci].u4TxRetryMsdu;

					/*for dump bss statistics */
					prStaRec->arLinkStatistics[eAci].u4TxFailMsdu =
					    prEvent->arLinkStatistics[eAci].u4TxFailMsdu;
					prStaRec->arLinkStatistics[eAci].u4TxRetryMsdu =
					    prEvent->arLinkStatistics[eAci].u4TxRetryMsdu;
				}
			}
			if (prEvent->u4TxCount) {
				uint32_t u4TxDoneAirTimeMs = USEC_TO_MSEC(prEvent->u4TxDoneAirTime * 32);

				prStaStatistics->u4TxAverageAirTime = (u4TxDoneAirTimeMs / prEvent->u4TxCount);
			} else {
				prStaStatistics->u4TxAverageAirTime = 0;
			}
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}

}

#if CFG_AUTO_CHANNEL_SEL_SUPPORT
/*----------------------------------------------------------------------------*/
/*!
* @brief This function is called when event for query LTE safe channels
*        has been retrieved
*
* @param prAdapter          Pointer to the Adapter structure.
* @param prCmdInfo          Pointer to the command information
* @param pucEventBuf        Pointer to the event buffer
*
* @return none
*/
/*----------------------------------------------------------------------------*/
void nicCmdEventQueryLteSafeChn(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	uint32_t u4QueryInfoLen;
	struct EVENT_LTE_SAFE_CHN *prEvent;
	struct GLUE_INFO *prGlueInfo;
	struct PARAM_GET_CHN_INFO *prLteSafeChnInfo;
	uint8_t ucIdx = 0;

	if ((prAdapter == NULL)
		|| (prCmdInfo == NULL)
		|| (pucEventBuf == NULL)
		|| (prCmdInfo->pvInformationBuffer == NULL)) {
		ASSERT(FALSE);
		return;
	}

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;
		prEvent = (struct EVENT_LTE_SAFE_CHN *) pucEventBuf;	/* FW responsed data */

		prLteSafeChnInfo = (struct PARAM_GET_CHN_INFO *) prCmdInfo->pvInformationBuffer;

		u4QueryInfoLen = sizeof(struct PARAM_GET_CHN_INFO);

		/* Statistics from FW is valid */
		if (prEvent->u4Flags & BIT(0)) {
			for (ucIdx = 0; ucIdx < NL80211_TESTMODE_AVAILABLE_CHAN_ATTR_MAX; ucIdx++) {
				prLteSafeChnInfo->rLteSafeChnList.au4SafeChannelBitmask[ucIdx] =
					prEvent->rLteSafeChn.au4SafeChannelBitmask[ucIdx];

				DBGLOG(P2P, TRACE,
				       "[ACS]LTE safe channels[%d]=0x%08x\n", ucIdx,
				       prLteSafeChnInfo->rLteSafeChnList.au4SafeChannelBitmask[ucIdx]);
			}
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}
#endif

void nicEventRddPulseDump(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	uint16_t u2Idx, u2PulseCnt;
	struct EVENT_WIFI_RDD_TEST *prRddPulseEvent;

	ASSERT(prAdapter);
	ASSERT(pucEventBuf);

	prRddPulseEvent = (struct EVENT_WIFI_RDD_TEST *) (pucEventBuf);

	u2PulseCnt = (prRddPulseEvent->u4FuncLength - RDD_EVENT_HDR_SIZE) / RDD_ONEPLUSE_SIZE;

	DBGLOG(INIT, INFO, "[RDD]0x%08x %08d[RDD%d]\n", prRddPulseEvent->u4Prefix
			, prRddPulseEvent->u4Count, prRddPulseEvent->ucRddIdx);

	for (u2Idx = 0; u2Idx < u2PulseCnt; u2Idx++) {
		DBGLOG(INIT, INFO, "[RDD]0x%02x%02x%02x%02x %02x%02x%02x%02x[RDD%d]\n"
			, prRddPulseEvent->aucBuffer[RDD_ONEPLUSE_SIZE*u2Idx+RDD_PULSE_OFFSET3]
			, prRddPulseEvent->aucBuffer[RDD_ONEPLUSE_SIZE*u2Idx+RDD_PULSE_OFFSET2]
			, prRddPulseEvent->aucBuffer[RDD_ONEPLUSE_SIZE*u2Idx+RDD_PULSE_OFFSET1]
			, prRddPulseEvent->aucBuffer[RDD_ONEPLUSE_SIZE*u2Idx+RDD_PULSE_OFFSET0]
			, prRddPulseEvent->aucBuffer[RDD_ONEPLUSE_SIZE*u2Idx+RDD_PULSE_OFFSET7]
			, prRddPulseEvent->aucBuffer[RDD_ONEPLUSE_SIZE*u2Idx+RDD_PULSE_OFFSET6]
			, prRddPulseEvent->aucBuffer[RDD_ONEPLUSE_SIZE*u2Idx+RDD_PULSE_OFFSET5]
			, prRddPulseEvent->aucBuffer[RDD_ONEPLUSE_SIZE*u2Idx+RDD_PULSE_OFFSET4]
			, prRddPulseEvent->ucRddIdx
			);
	}

}

#if CFG_SUPPORT_MSP
void nicCmdEventQueryWlanInfo(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct PARAM_HW_WLAN_INFO *prWlanInfo;
	struct EVENT_WLAN_INFO *prEventWlanInfo;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventWlanInfo = (struct EVENT_WLAN_INFO *) pucEventBuf;

	DBGLOG(RSN, INFO, "MT6632 : nicCmdEventQueryWlanInfo\n");

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		u4QueryInfoLen = sizeof(struct PARAM_HW_WLAN_INFO);
		prWlanInfo = (struct PARAM_HW_WLAN_INFO *) prCmdInfo->pvInformationBuffer;

		/* prWlanInfo->u4Length = sizeof(PARAM_HW_WLAN_INFO_T); */
		if (prEventWlanInfo && prWlanInfo) {
			kalMemCopy(&prWlanInfo->rWtblTxConfig,
				   &prEventWlanInfo->rWtblTxConfig,
				   sizeof(struct PARAM_TX_CONFIG));
			kalMemCopy(&prWlanInfo->rWtblSecConfig,
				   &prEventWlanInfo->rWtblSecConfig,
				   sizeof(struct PARAM_SEC_CONFIG));
			kalMemCopy(&prWlanInfo->rWtblKeyConfig,
				   &prEventWlanInfo->rWtblKeyConfig,
				   sizeof(struct PARAM_KEY_CONFIG));
			kalMemCopy(&prWlanInfo->rWtblRateInfo,
				   &prEventWlanInfo->rWtblRateInfo,
				   sizeof(struct PARAM_PEER_RATE_INFO));
			kalMemCopy(&prWlanInfo->rWtblBaConfig,
				   &prEventWlanInfo->rWtblBaConfig,
				   sizeof(struct PARAM_PEER_BA_CONFIG));
			kalMemCopy(&prWlanInfo->rWtblPeerCap,
				   &prEventWlanInfo->rWtblPeerCap,
				   sizeof(struct PARAM_PEER_CAP));
			kalMemCopy(&prWlanInfo->rWtblRxCounter,
				   &prEventWlanInfo->rWtblRxCounter,
				   sizeof(struct PARAM_PEER_RX_COUNTER_ALL));
			kalMemCopy(&prWlanInfo->rWtblTxCounter,
					&prEventWlanInfo->rWtblTxCounter,
					sizeof(struct PARAM_PEER_TX_COUNTER_ALL));
		}
		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}


void nicCmdEventQueryMibInfo(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct PARAM_HW_MIB_INFO *prMibInfo;
	struct EVENT_MIB_INFO *prEventMibInfo;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prEventMibInfo = (struct EVENT_MIB_INFO *) pucEventBuf;

	DBGLOG(RSN, INFO, "MT6632 : nicCmdEventQueryMibInfo\n");

	if (prCmdInfo->fgIsOid) {
		prGlueInfo = prAdapter->prGlueInfo;

		u4QueryInfoLen = sizeof(struct PARAM_HW_MIB_INFO);
		prMibInfo = (struct PARAM_HW_MIB_INFO *) prCmdInfo->pvInformationBuffer;
		if (prEventMibInfo && prMibInfo) {
			kalMemCopy(&prMibInfo->rHwMibCnt,
				   &prEventMibInfo->rHwMibCnt,
				   sizeof(struct HW_MIB_COUNTER));
			kalMemCopy(&prMibInfo->rHwMib2Cnt,
				   &prEventMibInfo->rHwMib2Cnt,
				   sizeof(struct HW_MIB2_COUNTER));
			kalMemCopy(&prMibInfo->rHwTxAmpduMts,
				   &prEventMibInfo->rHwTxAmpduMts,
				   sizeof(struct HW_TX_AMPDU_METRICS));
		}
		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}
#endif

uint32_t nicEventQueryTxResourceEntry(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct NIC_TX_RESOURCE *prTxResource;
	uint32_t version = *((uint32_t *)pucEventBuf);


	prAdapter->fgIsNicTxReousrceValid = TRUE;

	if (version & NIC_TX_RESOURCE_REPORT_VERSION_PREFIX)
		return nicEventQueryTxResource(prAdapter, pucEventBuf);

	/* 6632, 7668 ways */
	prAdapter->nicTxReousrce.txResourceInit = NULL;

	prTxResource = (struct NIC_TX_RESOURCE *)pucEventBuf;
	prAdapter->nicTxReousrce.u4CmdTotalResource = prTxResource->u4CmdTotalResource;
	prAdapter->nicTxReousrce.u4CmdResourceUnit = prTxResource->u4CmdResourceUnit;
	prAdapter->nicTxReousrce.u4DataTotalResource = prTxResource->u4DataTotalResource;
	prAdapter->nicTxReousrce.u4DataResourceUnit = prTxResource->u4DataResourceUnit;

	DBGLOG(INIT, INFO, "nicCmdEventQueryNicTxResource: u4CmdTotalResource = %x\n",
						prAdapter->nicTxReousrce.u4CmdTotalResource);
	DBGLOG(INIT, INFO, "nicCmdEventQueryNicTxResource: u4CmdResourceUnit = %x\n",
						prAdapter->nicTxReousrce.u4CmdResourceUnit);
	DBGLOG(INIT, INFO, "nicCmdEventQueryNicTxResource: u4DataTotalResource = %x\n",
						prAdapter->nicTxReousrce.u4DataTotalResource);
	DBGLOG(INIT, INFO, "nicCmdEventQueryNicTxResource: u4DataResourceUnit = %x\n",
						prAdapter->nicTxReousrce.u4DataResourceUnit);

	return WLAN_STATUS_SUCCESS;
}

uint32_t nicCmdEventQueryNicEfuseAddr(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct NIC_EFUSE_ADDRESS *prTxResource = (struct NIC_EFUSE_ADDRESS *)pucEventBuf;

	prAdapter->u4EfuseStartAddress = prTxResource->u4EfuseStartAddress;
	prAdapter->u4EfuseEndAddress = prTxResource->u4EfuseEndAddress;

	DBGLOG(INIT, INFO, "nicCmdEventQueryNicEfuseAddr: u4EfuseStartAddress = %x\n",
						prAdapter->u4EfuseStartAddress);
	DBGLOG(INIT, INFO, "nicCmdEventQueryNicEfuseAddr: u4EfuseEndAddress = %x\n",
						prAdapter->u4EfuseEndAddress);

	return WLAN_STATUS_SUCCESS;
}

uint32_t nicCmdEventQueryNicCoexFeature(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct NIC_COEX_FEATURE *prCoexFeature = (struct NIC_COEX_FEATURE *)pucEventBuf;

	prAdapter->u4FddMode = prCoexFeature->u4FddMode;

	DBGLOG(INIT, INFO, "nicCmdEventQueryNicCoexFeature: u4FddMode = %x\n",
						prAdapter->u4FddMode);

	return WLAN_STATUS_SUCCESS;
}

#if CFG_TCP_IP_CHKSUM_OFFLOAD
uint32_t nicCmdEventQueryNicCsumOffload(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct NIC_CSUM_OFFLOAD *prChecksumOffload = (struct NIC_CSUM_OFFLOAD *)pucEventBuf;

	prAdapter->fgIsSupportCsumOffload = prChecksumOffload->ucIsSupportCsumOffload;

	DBGLOG(INIT, INFO, "nicCmdEventQueryNicCsumOffload: ucIsSupportCsumOffload = %x\n",
						prAdapter->fgIsSupportCsumOffload);

	return WLAN_STATUS_SUCCESS;
}
#endif

uint32_t nicCfgChipCapHwVersion(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct CAP_HW_VERSION_T *prHwVer = (struct CAP_HW_VERSION_T *)pucEventBuf;

	prAdapter->rVerInfo.u2FwProductID = prHwVer->u2ProductID;

	return WLAN_STATUS_SUCCESS;
}

uint32_t nicCfgChipCapSwVersion(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct CAP_SW_VERSION_T *prSwVer = (struct CAP_SW_VERSION_T *)pucEventBuf;

	prAdapter->rVerInfo.u2FwOwnVersion = prSwVer->u2FwVersion;
	prAdapter->rVerInfo.ucFwBuildNumber = prSwVer->u2FwBuildNumber;
	kalMemCopy(prAdapter->rVerInfo.aucFwBranchInfo, prSwVer->aucBranchInfo, 4);
	kalMemCopy(prAdapter->rVerInfo.aucFwDateCode, prSwVer->aucDateCode, 16);

return WLAN_STATUS_SUCCESS;
}

uint32_t nicCfgChipCapMacAddr(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct CAP_MAC_ADDR_T *prMacAddr = (struct CAP_MAC_ADDR_T *)pucEventBuf;
	uint8_t aucZeroMacAddr[] = NULL_MAC_ADDR;

	COPY_MAC_ADDR(prAdapter->rWifiVar.aucPermanentAddress, prMacAddr->aucMacAddr);
	COPY_MAC_ADDR(prAdapter->rWifiVar.aucMacAddress, prMacAddr->aucMacAddr);
	prAdapter->fgIsEmbbededMacAddrValid = (u_int8_t) (!IS_BMCAST_MAC_ADDR(prMacAddr->aucMacAddr) &&
						 !EQUAL_MAC_ADDR(aucZeroMacAddr, prMacAddr->aucMacAddr));

	return WLAN_STATUS_SUCCESS;
}

uint32_t nicCfgChipCapPhyCap(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct CAP_PHY_CAP_T *prPhyCap = (struct CAP_PHY_CAP_T *)pucEventBuf;

	prAdapter->rWifiVar.ucStaVht &= prPhyCap->ucVht;
	prAdapter->rWifiVar.ucApVht &= prPhyCap->ucVht;
	prAdapter->rWifiVar.ucP2pGoVht &= prPhyCap->ucVht;
	prAdapter->rWifiVar.ucP2pGcVht &= prPhyCap->ucVht;
	prAdapter->fgIsHw5GBandDisabled = !prPhyCap->uc5gBand;
	prAdapter->rWifiVar.ucNSS = prPhyCap->ucNss;
#if CFG_SUPPORT_DBDC
	if (!prPhyCap->ucDbdc)
		prAdapter->rWifiVar.eDbdcMode = ENUM_DBDC_MODE_DISABLED;
#endif

	return WLAN_STATUS_SUCCESS;
}

uint32_t nicCfgChipCapMacCap(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct CAP_MAC_CAP_T *prMacCap = (struct CAP_MAC_CAP_T *)pucEventBuf;

	if (prMacCap->ucHwBssIdNum > 0 && prMacCap->ucHwBssIdNum <= MAX_BSSID_NUM) {
		prAdapter->ucHwBssIdNum = prMacCap->ucHwBssIdNum;
		prAdapter->ucP2PDevBssIdx = prAdapter->ucHwBssIdNum;
		prAdapter->aprBssInfo[prAdapter->ucP2PDevBssIdx] = &prAdapter->rWifiVar.rP2pDevInfo;
	}
	DBGLOG(INIT, INFO, "ucHwBssIdNum: %d.\n", prMacCap->ucHwBssIdNum);

	if (prMacCap->ucWtblEntryNum > 0 && prMacCap->ucWtblEntryNum <= WTBL_SIZE) {
		prAdapter->ucWtblEntryNum = prMacCap->ucWtblEntryNum;
		prAdapter->ucTxDefaultWlanIndex = prAdapter->ucWtblEntryNum - 1;
	}
	DBGLOG(INIT, INFO, "ucWtblEntryNum: %d.\n", prMacCap->ucWtblEntryNum);

	return WLAN_STATUS_SUCCESS;
}

uint32_t nicCfgChipCapFrameBufCap(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	/* struct CAP_FRAME_BUF_CAP_T *prFrameBuf = (struct CAP_FRAME_BUF_CAP_T *)pucEventBuf; */

	return WLAN_STATUS_SUCCESS;
}

uint32_t nicCfgChipCapBeamformCap(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	/* struct CAP_BEAMFORM_CAP_T *prBeamform = (struct CAP_BEAMFORM_CAP_T *)pucEventBuf; */

	return WLAN_STATUS_SUCCESS;
}

uint32_t nicCfgChipCapLocationCap(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	/* struct CAP_LOCATION_CAP_T *prLocation = (struct CAP_LOCATION_CAP_T *)pucEventBuf; */

	return WLAN_STATUS_SUCCESS;
}

uint32_t nicCfgChipCapMuMimoCap(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	/* struct CAP_MUMIMO_CAP_T *prMuMimo = (struct CAP_HW_VERSION_T *)pucEventBuf; */

	return WLAN_STATUS_SUCCESS;
}

struct nicTxRsrcEvtHdlr nicTxRsrcEvtHdlrTbl[] = {
	{NIC_TX_RESOURCE_REPORT_VERSION_1, nicEventQueryTxResource_v1, nicTxResourceUpdate_v1},
};

uint32_t nicEventQueryTxResource(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	uint32_t i, i_max;
	uint32_t version = *((uint32_t *)(pucEventBuf));

	i_max = sizeof(nicTxRsrcEvtHdlrTbl)/sizeof(struct nicTxRsrcEvtHdlr);
	for (i = 0; i < i_max; i++) {
		if (version == nicTxRsrcEvtHdlrTbl[i].u4Version) {
			/* assign callback to do the resource init. */
			prAdapter->nicTxReousrce.txResourceInit = nicTxRsrcEvtHdlrTbl[i].nicTxResourceInit;

			return nicTxRsrcEvtHdlrTbl[i].nicEventTxResource(prAdapter, pucEventBuf);
		}

		/* move to next index*/
		i++;
	}

	/* invalid version, cannot find the handler */
	DBGLOG(INIT, ERROR, "nicEventQueryTxResource(): Invaalid version.\n");
	prAdapter->nicTxReousrce.txResourceInit = NULL;

	return WLAN_STATUS_NOT_SUPPORTED;
}

uint32_t nicEventQueryTxResource_v1(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct tx_resource_report_v1 *pV1 = (struct tx_resource_report_v1 *)pucEventBuf;
	uint16_t page_size;

	/* PSE */
	page_size = pV1->u4PlePsePageSize & 0xFFFF;
	prAdapter->nicTxReousrce.u4CmdTotalResource = pV1->u4HifCmdPsePageQuota;
	prAdapter->nicTxReousrce.u4CmdResourceUnit = page_size;
	prAdapter->nicTxReousrce.u4DataTotalResource = pV1->u4HifDataPsePageQuota;
	prAdapter->nicTxReousrce.u4DataResourceUnit = page_size;

	/* PLE */
	page_size = (pV1->u4PlePsePageSize >> 16) & 0xFFFF;
	prAdapter->nicTxReousrce.u4CmdTotalResourcePle = pV1->u4HifCmdPlePageQuota;
	prAdapter->nicTxReousrce.u4CmdResourceUnitPle = page_size;
	prAdapter->nicTxReousrce.u4DataTotalResourcePle = pV1->u4HifDataPlePageQuota;
	prAdapter->nicTxReousrce.u4DataResourceUnitPle = page_size;

	/* PpTxAddCnt */
	prAdapter->nicTxReousrce.ucPpTxAddCnt = pV1->ucPpTxAddCnt;

	/* enable PLE resource control flag */
	if (!prAdapter->nicTxReousrce.u4DataTotalResourcePle)
		prAdapter->rTxCtrl.rTc.fgNeedPleCtrl = FALSE;
	else
		prAdapter->rTxCtrl.rTc.fgNeedPleCtrl = NIC_TX_RESOURCE_CTRL_PLE;
	return WLAN_STATUS_SUCCESS;
}

void nicCmdEventQueryNicCapabilityV2(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct EVENT_NIC_CAPABILITY_V2 *prEventNicV2 = (struct EVENT_NIC_CAPABILITY_V2 *)pucEventBuf;
	struct NIC_CAPABILITY_V2_ELEMENT *prElement;
	uint32_t tag_idx, table_idx, offset;

	offset = 0;

	/* process each element */
	for (tag_idx = 0; tag_idx < prEventNicV2->u2TotalElementNum; tag_idx++) {

		prElement = (struct NIC_CAPABILITY_V2_ELEMENT *)(prEventNicV2->aucBuffer + offset);

		for (table_idx = 0;
			 table_idx < (sizeof(gNicCapabilityV2InfoTable)/sizeof(struct NIC_CAPABILITY_V2_REF_TABLE));
			 table_idx++) {

			/* find the corresponding tag's handler */
			if (gNicCapabilityV2InfoTable[table_idx].tag_type == prElement->tag_type) {
				gNicCapabilityV2InfoTable[table_idx].hdlr(prAdapter, prElement->aucbody);
				break;
			}
		}

		/* move to the next tag */
		offset += prElement->body_len + (uint16_t) OFFSET_OF(struct NIC_CAPABILITY_V2_ELEMENT, aucbody);
	}

}

#if (CFG_SUPPORT_TXPOWER_INFO == 1)
void nicCmdEventQueryTxPowerInfo(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct EXT_EVENT_TXPOWER_ALL_RATE_POWER_INFO_T *prEvent = NULL;
	struct PARAM_TXPOWER_ALL_RATE_POWER_INFO_T *prTxPowerInfo = NULL;
	uint32_t u4QueryInfoLen;
	struct GLUE_INFO *prGlueInfo = prAdapter->prGlueInfo;

	if (!prAdapter)
		return;
	if (!prCmdInfo)
		return;
	if (!pucEventBuf)
		return;
	if (!(prCmdInfo->pvInformationBuffer))
		return;

	if (prCmdInfo->fgIsOid) {
		prEvent = (struct EXT_EVENT_TXPOWER_ALL_RATE_POWER_INFO_T *) pucEventBuf;

		if (prEvent->ucTxPowerCategory == TXPOWER_EVENT_SHOW_ALL_RATE_TXPOWER_INFO) {
			prEvent = (struct EXT_EVENT_TXPOWER_ALL_RATE_POWER_INFO_T *) pucEventBuf;
			prTxPowerInfo = (struct PARAM_TXPOWER_ALL_RATE_POWER_INFO_T *) prCmdInfo->pvInformationBuffer;
			u4QueryInfoLen = sizeof(struct PARAM_TXPOWER_ALL_RATE_POWER_INFO_T);

			kalMemCopy(prTxPowerInfo, prEvent, sizeof(struct EXT_EVENT_TXPOWER_ALL_RATE_POWER_INFO_T));
		}

		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}
#endif

void nicEventLinkQuality(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct CMD_INFO *prCmdInfo;

	ASSERT(prAdapter);

#if CFG_ENABLE_WIFI_DIRECT && CFG_SUPPORT_P2P_RSSI_QUERY
	if (prEvent->u2PacketLen == EVENT_HDR_WITHOUT_RXD_SIZE + sizeof(struct EVENT_LINK_QUALITY_EX)) {
		struct EVENT_LINK_QUALITY_EX *prLqEx = (struct EVENT_LINK_QUALITY_EX *) (prEvent->aucBuffer);

		if (prLqEx->ucIsLQ0Rdy)
			nicUpdateLinkQuality(prAdapter, 0, (struct EVENT_LINK_QUALITY *) prLqEx);
		if (prLqEx->ucIsLQ1Rdy)
			nicUpdateLinkQuality(prAdapter, 1, (struct EVENT_LINK_QUALITY *) prLqEx);
	} else {
		/* For old FW, P2P may invoke link quality query, and make driver flag becone TRUE. */
		DBGLOG(P2P, WARN, "Old FW version, not support P2P RSSI query.\n");

		/* Must not use NETWORK_TYPE_P2P_INDEX, cause the structure is mismatch. */
		nicUpdateLinkQuality(prAdapter, 0, (struct EVENT_LINK_QUALITY *) (prEvent->aucBuffer));
	}
#else
	/*only support ais query */
	{
		uint8_t ucBssIndex;
		struct BSS_INFO *prBssInfo;

		for (ucBssIndex = 0; ucBssIndex < prAdapter->ucHwBssIdNum; ucBssIndex++) {
			prBssInfo = prAdapter->aprBssInfo[ucBssIndex];

			if (prBssInfo->eNetworkType == NETWORK_TYPE_AIS && prBssInfo->fgIsInUse)
				break;
		}

		if (ucBssIndex >= prAdapter->ucHwBssIdNum)
			ucBssIndex = 1;	/* No hit(bss1 for default ais network) */
		/* printk("=======> rssi with bss%d ,%d\n",ucBssIndex,
		 * ((struct EVENT_LINK_QUALITY_V2 *)(prEvent->aucBuffer))->rLq[ucBssIndex].cRssi);
		 */
		nicUpdateLinkQuality(prAdapter, ucBssIndex, (struct EVENT_LINK_QUALITY_V2 *) (prEvent->aucBuffer));
	}

#endif

	/* command response handling */
	prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

	if (prCmdInfo != NULL) {
		if (prCmdInfo->pfCmdDoneHandler)
			prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
		else if (prCmdInfo->fgIsOid)
			kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
		/* return prCmdInfo */
		cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
	}
#ifndef LINUX
	if (prAdapter->rWlanInfo.eRssiTriggerType == ENUM_RSSI_TRIGGER_GREATER &&
		prAdapter->rWlanInfo.rRssiTriggerValue >= (int32_t) (prAdapter->rLinkQuality.cRssi)) {

		prAdapter->rWlanInfo.eRssiTriggerType = ENUM_RSSI_TRIGGER_TRIGGERED;

		kalIndicateStatusAndComplete(prAdapter->prGlueInfo, WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
			(void *)&(prAdapter->rWlanInfo.rRssiTriggerValue), sizeof(int32_t));
	} else if (prAdapter->rWlanInfo.eRssiTriggerType == ENUM_RSSI_TRIGGER_LESS &&
		prAdapter->rWlanInfo.rRssiTriggerValue <= (int32_t) (prAdapter->rLinkQuality.cRssi)) {

		prAdapter->rWlanInfo.eRssiTriggerType = ENUM_RSSI_TRIGGER_TRIGGERED;

		kalIndicateStatusAndComplete(prAdapter->prGlueInfo, WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
			(void *)&(prAdapter->rWlanInfo.rRssiTriggerValue), sizeof(int32_t));
	}
#endif
}

uint32_t nicExtTsfRawData2IqFmt(struct EXT_EVENT_RBIST_DUMP_DATA_T *prEventDumpMem, struct ICAP_INFO_T *prIcap)
{
	static uint8_t aucPathWF0[40];	/* the path for iq data dump out */
	static uint8_t aucPathWF1[40];	/* the path for iq data dump out */
	static uint8_t aucPathRAWWF0[40];	/* the path for iq data dump out */
	static uint8_t aucPathRAWWF1[40];	/* the path for iq data dump out */
	uint8_t *pucDataWF0 = NULL;	/* the data write into file */
	uint8_t *pucDataWF1 = NULL;	/* the data write into file */
	uint8_t *pucDataRAWWF0 = NULL;	/* the data write into file */
	uint8_t *pucDataRAWWF1 = NULL;	/* the data write into file */
	uint32_t u4SrcOffset;	/* record the buffer offset */
	uint32_t u4FmtLen = 0;	/* bus format length */
	uint32_t u4CpyLen = 0;
	uint32_t u4RemainByte;
	uint32_t u4DataWBufSize = 150;
	uint32_t u4DataRAWWBufSize = 150;
	uint32_t u4DataWLenF0 = 0;
	uint32_t u4DataWLenF1 = 0;
	uint32_t u4DataRAWWLenF0 = 0;
	uint32_t u4DataRAWWLenF1 = 0;
	u_int8_t fgAppend;
	int32_t u4Iqc160WF0Q0, u4Iqc160WF1I1;
	static uint8_t ucDstOffset;	/* for alignment. bcs we send 2KB data per packet,*/
					/* the data will not align in 12 bytes case. */
	static uint32_t u4CurTimeTick;

	static union ICAP_BUS_FMT icapBusData;
	uint32_t *ptr;

	pucDataWF0 = kmalloc(u4DataWBufSize, GFP_KERNEL);
	pucDataWF1 = kmalloc(u4DataWBufSize, GFP_KERNEL);
	pucDataRAWWF0 = kmalloc(u4DataRAWWBufSize, GFP_KERNEL);
	pucDataRAWWF1 = kmalloc(u4DataRAWWBufSize, GFP_KERNEL);


	if ((!pucDataWF0) || (!pucDataWF1) || (!pucDataRAWWF0) || (!pucDataRAWWF1)) {
		DBGLOG(INIT, ERROR, "kmalloc failed.\n");
		kfree(pucDataWF0);
		kfree(pucDataWF1);
		kfree(pucDataRAWWF0);
		kfree(pucDataRAWWF1);
		ASSERT(-1);
		return -1;
	}

	fgAppend = TRUE;
	if (prEventDumpMem->u4PktNum == 0) {
		u4CurTimeTick = kalGetTimeTick();
		/* Store memory dump into sdcard,
		 * path /sdcard/dump_<current  system tick>_<memory address>_<memory length>.hex
		 */
#if defined(LINUX)

		/*if blbist mkdir undre /data/blbist, the dump files wouls put on it */
		scnprintf(aucPathWF0, sizeof(aucPathWF0), "/dump_out_%05hu_WF0.txt", prIcap->u2DumpIndex);
		scnprintf(aucPathWF1, sizeof(aucPathWF1), "/dump_out_%05hu_WF1.txt", prIcap->u2DumpIndex);
		if (kalCheckPath(aucPathWF0) == -1) {
			kalMemSet(aucPathWF0, 0x00, sizeof(aucPathWF0));
			scnprintf(aucPathWF0, sizeof(aucPathWF0),
				  "/data/dump_out_%05hu_WF0.txt", prIcap->u2DumpIndex);
		} else
			kalTrunkPath(aucPathWF0);

		if (kalCheckPath(aucPathWF1) == -1) {
			kalMemSet(aucPathWF1, 0x00, sizeof(aucPathWF1));
			scnprintf(aucPathWF1, sizeof(aucPathWF1),
				  "/data/dump_out_%05hu_WF1.txt", prIcap->u2DumpIndex);
		} else
			kalTrunkPath(aucPathWF1);

		scnprintf(aucPathRAWWF0, sizeof(aucPathRAWWF0), "/dump_RAW_%05hu_WF0.txt", prIcap->u2DumpIndex);
		scnprintf(aucPathRAWWF1, sizeof(aucPathRAWWF1), "/dump_RAW_%05hu_WF1.txt", prIcap->u2DumpIndex);
		if (kalCheckPath(aucPathRAWWF0) == -1) {
			kalMemSet(aucPathRAWWF0, 0x00, sizeof(aucPathRAWWF0));
			scnprintf(aucPathRAWWF0, sizeof(aucPathRAWWF0),
				  "/data/dump_RAW_%05hu_WF0.txt", prIcap->u2DumpIndex);
		} else
			kalTrunkPath(aucPathRAWWF0);

		if (kalCheckPath(aucPathRAWWF1) == -1) {
			kalMemSet(aucPathRAWWF1, 0x00, sizeof(aucPathRAWWF1));
			scnprintf(aucPathRAWWF1, sizeof(aucPathRAWWF1),
				  "/data/dump_RAW_%hu_WF1.txt", prIcap->u2DumpIndex);
		} else
			kalTrunkPath(aucPathRAWWF1);

#else
		/* TODO: check Address */
		kal_sprintf_ddk(aucPathWF0, sizeof(aucPathWF0),
				u4CurTimeTick, prEventDumpMem->u4Address,
				prEventDumpMem->u4Length + prEventDumpMem->u4RemainLength);
		kal_sprintf_ddk(aucPathWF1, sizeof(aucPathWF1),
				u4CurTimeTick, prEventDumpMem->u4Address,
				prEventDumpMem->u4Length + prEventDumpMem->u4RemainLength);
#endif
		/* fgAppend = FALSE; */
	}

	ptr = (uint32_t *)(&prEventDumpMem->u4Data);

	for (u4SrcOffset = 0, u4RemainByte = prEventDumpMem->u4DataLength; u4RemainByte > 0;) {
		u4FmtLen = (prIcap->u4CapNode == ICAP_CONTENT_SPECTRUM) ?
			sizeof(struct SPECTRUM_BUS_FMT) : sizeof(union ICAP_BUS_FMT);
		/* 4 bytes : 12 bytes */
		u4CpyLen = (u4RemainByte - u4FmtLen >= 0) ? u4FmtLen : u4RemainByte;
		if ((ucDstOffset + u4CpyLen) > sizeof(icapBusData)) {
			DBGLOG(INIT, ERROR,
			       "ucDstOffset(%u) + u4CpyLen(%u) exceed bound of icapBusData\n",
			       ucDstOffset, u4CpyLen);
			kfree(pucDataWF0);
			kfree(pucDataWF1);
			kfree(pucDataRAWWF0);
			kfree(pucDataRAWWF1);
			ASSERT(-1);
			return -1;
		}

		memcpy((uint8_t *)&icapBusData + ucDstOffset, &prEventDumpMem->u4Data + u4SrcOffset, u4CpyLen);
		if (prIcap->u4CapNode == ICAP_CONTENT_FIIQ || prIcap->u4CapNode == ICAP_CONTENT_FDIQ) {
			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize, "%8d,%8d\n",
						 icapBusData.rIqcBusData.u4Iqc0I,
						 icapBusData.rIqcBusData.u4Iqc0Q);
			u4DataWLenF1 = scnprintf(pucDataWF1, u4DataWBufSize, "%8d,%8d\n",
						 icapBusData.rIqcBusData.u4Iqc1I,
						 icapBusData.rIqcBusData.u4Iqc1Q);
		} else if (prIcap->u4CapNode - 1000 == ICAP_CONTENT_FIIQ ||
			   prIcap->u4CapNode - 1000 == ICAP_CONTENT_FDIQ) {
			u4Iqc160WF0Q0 = icapBusData.rIqc160BusData.u4Iqc0Q0P1 |
				(icapBusData.rIqc160BusData.u4Iqc0Q0P2 << 8);
			u4Iqc160WF1I1 = icapBusData.rIqc160BusData.u4Iqc1I1P1 |
				(icapBusData.rIqc160BusData.u4Iqc1I1P2 << 4);

			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize, "%8d,%8d\n%8d,%8d\n",
						 icapBusData.rIqc160BusData.u4Iqc0I0, u4Iqc160WF0Q0,
						 icapBusData.rIqc160BusData.u4Iqc0I1,
						 icapBusData.rIqc160BusData.u4Iqc0Q1);

			u4DataWLenF1 = scnprintf(pucDataWF1, u4DataWBufSize, "%8d,%8d\n%8d,%8d\n",
						 icapBusData.rIqc160BusData.u4Iqc1I0,
						 icapBusData.rIqc160BusData.u4Iqc1Q0, u4Iqc160WF1I1,
						 icapBusData.rIqc160BusData.u4Iqc1Q1);

		} else if (prIcap->u4CapNode == ICAP_CONTENT_SPECTRUM) {
			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize, "%8d,%8d\n",
						 icapBusData.rSpectrumBusData.u4DcocI,
						 icapBusData.rSpectrumBusData.u4DcocQ);
		} else if (prIcap->u4CapNode == ICAP_CONTENT_ADC) {
			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize,
						 "%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n",
						 icapBusData.rPackedAdcBusData.u4AdcI0T0,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T0,
						 icapBusData.rPackedAdcBusData.u4AdcI0T1,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T1,
						 icapBusData.rPackedAdcBusData.u4AdcI0T2,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T2,
						 icapBusData.rPackedAdcBusData.u4AdcI0T3,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T3,
						 icapBusData.rPackedAdcBusData.u4AdcI0T4,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T4,
						 icapBusData.rPackedAdcBusData.u4AdcI0T5,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T5);

			u4DataWLenF1 = scnprintf(pucDataWF1, u4DataWBufSize,
						 "%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n%8d,%8d\n",
						 icapBusData.rPackedAdcBusData.u4AdcI1T0,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T0,
						 icapBusData.rPackedAdcBusData.u4AdcI1T1,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T1,
						 icapBusData.rPackedAdcBusData.u4AdcI1T2,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T2,
						 icapBusData.rPackedAdcBusData.u4AdcI1T3,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T3,
						 icapBusData.rPackedAdcBusData.u4AdcI1T4,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T4,
						 icapBusData.rPackedAdcBusData.u4AdcI1T5,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T5);
		} else if (prIcap->u4CapNode - 2000 == ICAP_CONTENT_ADC) {
			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize, "%8d,%8d\n%8d,%8d\n%8d,%8d\n",
						 icapBusData.rPackedAdcBusData.u4AdcI0T0,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T0,
						 icapBusData.rPackedAdcBusData.u4AdcI0T1,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T1,
						 icapBusData.rPackedAdcBusData.u4AdcI0T2,
						 icapBusData.rPackedAdcBusData.u4AdcQ0T2);

			u4DataWLenF1 = scnprintf(pucDataWF1, u4DataWBufSize, "%8d,%8d\n%8d,%8d\n%8d,%8d\n",
						 icapBusData.rPackedAdcBusData.u4AdcI1T0,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T0,
						 icapBusData.rPackedAdcBusData.u4AdcI1T1,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T1,
						 icapBusData.rPackedAdcBusData.u4AdcI1T2,
						 icapBusData.rPackedAdcBusData.u4AdcQ1T2);
		} else if (prIcap->u4CapNode == ICAP_CONTENT_TOAE) {
			/* actually, this is DCOC. we take TOAE as DCOC */
			u4DataWLenF0 = scnprintf(pucDataWF0, u4DataWBufSize, "%8d,%8d\n",
						 icapBusData.rAdcBusData.u4Dcoc0I, icapBusData.rAdcBusData.u4Dcoc0Q);
			u4DataWLenF1 = scnprintf(pucDataWF1, u4DataWBufSize, "%8d,%8d\n",
						 icapBusData.rAdcBusData.u4Dcoc1I, icapBusData.rAdcBusData.u4Dcoc1Q);
		}
		if (u4CpyLen == u4FmtLen) {	/* the data format is complete */
			kalWriteToFile(aucPathWF0, fgAppend, pucDataWF0, u4DataWLenF0);
			kalWriteToFile(aucPathWF1, fgAppend, pucDataWF1, u4DataWLenF1);
		}
		ptr = (uint32_t *)(&prEventDumpMem->u4Data + u4SrcOffset);
		u4DataRAWWLenF0 = scnprintf(pucDataRAWWF0, u4DataWBufSize, "%08x%08x%08x\n",
					    *(ptr + 2), *(ptr + 1), *ptr);
		kalWriteToFile(aucPathRAWWF0, fgAppend, pucDataRAWWF0, u4DataRAWWLenF0);
		kalWriteToFile(aucPathRAWWF1, fgAppend, pucDataRAWWF1, u4DataRAWWLenF1);

		u4RemainByte -= u4CpyLen;
		u4SrcOffset += u4CpyLen;	/* shift offset */
		ucDstOffset = 0;	/* only use ucDstOffset at first packet for align 2KB */
	}
	/* if this is a last packet, we can't transfer the remain data.
	 *  bcs we can't guarantee the data is complete align data format
	 */
	if (u4CpyLen != u4FmtLen) {	/* the data format is complete */
		ucDstOffset = u4CpyLen;	/* not align 2KB, keep the data and next packet data will append it */
	}

	kfree(pucDataWF0);
	kfree(pucDataWF1);
	kfree(pucDataRAWWF0);
	kfree(pucDataRAWWF1);

	if (u4RemainByte < 0) {
		ASSERT(-1);
		return -1;
	}

	return 0;
}

void nicExtEventICapIQData(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct EXT_EVENT_RBIST_DUMP_DATA_T *prICapEvent;
	uint32_t Idxi = 0, Idxj = 0, Idxk = 0;
	struct _RBIST_IQ_DATA_T *prIQArray = NULL;
	struct ICAP_INFO_T *prIcapInfo = NULL;

	ASSERT(prAdapter);
	ASSERT(pucEventBuf);

	prICapEvent = (struct EXT_EVENT_RBIST_DUMP_DATA_T *)pucEventBuf;
	prIcapInfo = &prAdapter->rIcapInfo;
	prIQArray = prIcapInfo->prIQArray;
	ASSERT(prIQArray);

	/* If we receive the packet which is delivered from last time data-capure, we need to drop it. */
	DBGLOG(RFTEST, INFO, "prICapEvent->u4PktNum = %d\n", prICapEvent->u4PktNum);
	if (prICapEvent->u4PktNum > prIcapInfo->u4ICapEventCnt) {
		if (prICapEvent->u4DataLength == 0)
			prAdapter->rIcapInfo.fgCaptureDone = TRUE;
		DBGLOG(RFTEST, ERROR, "Packet out of order: Pkt num %d, EventCnt %d\n",
				prICapEvent->u4PktNum, prIcapInfo->u4ICapEventCnt);
		return;
	}

	DBGLOG(RFTEST, INFO, "u4SmplCnt = [%d], u4WFCnt = [%d], IQArrayIndex = [%d]\n",
				prICapEvent->u4SmplCnt,
				prICapEvent->u4WFCnt,
				prIcapInfo->u4IQArrayIndex);

	if (prICapEvent->u4DataLength != 0 &&
	    prICapEvent->u4SmplCnt * prICapEvent->u4WFCnt * NUM_OF_CAP_TYPE > ICAP_EVENT_DATA_SAMPLE) {
		/* Max count = Total ICAP_EVENT_DATA_SAMPLE count and cut into half (I/Q) */
		prICapEvent->u4SmplCnt = ICAP_EVENT_DATA_SAMPLE / NUM_OF_CAP_TYPE;
		DBGLOG(RFTEST, WARN, "u4SmplCnt is larger than buffer size\n");
	}

	if (prIcapInfo->u4IQArrayIndex + prICapEvent->u4SmplCnt >= MAX_ICAP_IQ_DATA_CNT) {
		DBGLOG(RFTEST, ERROR, "Too many packets from FW, skip rest of them\n");
		return;
	}

	for (Idxi = 0; Idxi < prICapEvent->u4SmplCnt; Idxi++) {
		for (Idxj = 0; Idxj < prICapEvent->u4WFCnt; Idxj++) {
			prIQArray[prIcapInfo->u4IQArrayIndex].u4IQArray[Idxj][CAP_I_TYPE] = prICapEvent->u4Data[Idxk++];
			prIQArray[prIcapInfo->u4IQArrayIndex].u4IQArray[Idxj][CAP_Q_TYPE] = prICapEvent->u4Data[Idxk++];
		}
		prIcapInfo->u4IQArrayIndex++;
	}

	/* Print ICap data to console for debugging purpose */
	for (Idxi = 0; Idxi < ICAP_EVENT_DATA_SAMPLE; Idxi++)
		DBGLOG(RFTEST, TRACE, "Data[%d] : %x\n", Idxi, prICapEvent->u4Data[Idxi]);


	/* Update ICapEventCnt */
	if (prICapEvent->u4DataLength != 0)
		prIcapInfo->u4ICapEventCnt++;

	/* Check whether is the last FW event or not */
	if ((prICapEvent->u4DataLength == 0)
		&& (prICapEvent->u4PktNum == prIcapInfo->u4ICapEventCnt)) {
		/* Reset ICapEventCnt */
		prAdapter->rIcapInfo.fgIcapEnable = FALSE;
		prAdapter->rIcapInfo.fgCaptureDone = TRUE;
		prIcapInfo->u4ICapEventCnt = 0;
		DBGLOG(INIT, INFO, ": ==> gen done_file\n");
	}
}

void nicExtEventQueryMemDump(IN struct ADAPTER *prAdapter, IN uint8_t *pucEventBuf)
{
	struct EXT_EVENT_RBIST_DUMP_DATA_T *prEventDumpMem;
	static uint8_t aucPath[256];
	static uint8_t aucPath_done[300];
	static uint32_t u4CurTimeTick;

	ASSERT(prAdapter);
	ASSERT(pucEventBuf);

	sprintf(aucPath, "/dump_%05hu.hex", prAdapter->rIcapInfo.u2DumpIndex);

	prEventDumpMem = (struct EXT_EVENT_RBIST_DUMP_DATA_T *)pucEventBuf;

	if (kalCheckPath(aucPath) == -1) {
		kalMemSet(aucPath, 0x00, 256);
		sprintf(aucPath, "/data/dump_%05hu.hex", prAdapter->rIcapInfo.u2DumpIndex);
	}

	if (prEventDumpMem->u4PktNum == 0) {
		/* Store memory dump into sdcard,
		 * path /sdcard/dump_<current  system tick>_<memory address>_<memory length>.hex
		 */
		u4CurTimeTick = kalGetTimeTick();
#if defined(LINUX)

		/*if blbist mkdir undre /data/blbist, the dump files wouls put on it */
		sprintf(aucPath, "/dump_%05hu.hex", prAdapter->rIcapInfo.u2DumpIndex);
		if (kalCheckPath(aucPath) == -1) {
			kalMemSet(aucPath, 0x00, 256);
			sprintf(aucPath, "/data/dump_%05hu.hex", prAdapter->rIcapInfo.u2DumpIndex);
		}
#else
		/* TODO: check Address */
		kal_sprintf_ddk(aucPath, sizeof(aucPath),
				u4CurTimeTick, prEventDumpMem->u4Address,
				prEventDumpMem->u4Length + prEventDumpMem->u4RemainLength);
#endif
		kalWriteToFile(aucPath, FALSE, (uint8_t *)prEventDumpMem->u4Data, prEventDumpMem->u4DataLength);
	} else {
		/* Append current memory dump to the hex file */
		kalWriteToFile(aucPath, TRUE, (uint8_t *)prEventDumpMem->u4Data, prEventDumpMem->u4DataLength);
	}
#if CFG_SUPPORT_QA_TOOL
	nicExtTsfRawData2IqFmt(prEventDumpMem, &prAdapter->rIcapInfo);
#endif /* CFG_SUPPORT_QA_TOOL */

	/* TODO: check Address */
	/* DBGLOG(INIT, INFO, "iCap : ==> (u4DataLength = %x, u4Address=%x )\n", */
	/*       prEventDumpMem->u4DataLength, prEventDumpMem->u4Address); */


	if (prEventDumpMem->u4DataLength == 0) {
		/* The request is finished or firmware response a error */
		/* Reply time tick to iwpriv */

		prAdapter->rIcapInfo.fgIcapEnable = FALSE;
		prAdapter->rIcapInfo.fgCaptureDone = TRUE;

		sprintf(aucPath_done, "/file_dump_done.txt");
		if (kalCheckPath(aucPath_done) == -1) {
			kalMemSet(aucPath_done, 0x00, 256);
			sprintf(aucPath_done, "/data/file_dump_done.txt");
		}
		DBGLOG(INIT, INFO, ": ==> gen done_file\n");
		kalWriteToFile(aucPath_done, FALSE, aucPath_done, sizeof(aucPath_done));
#if CFG_SUPPORT_QA_TOOL
		prAdapter->rIcapInfo.au4Offset[0][0] = 0;
		prAdapter->rIcapInfo.au4Offset[0][1] = 9;
		prAdapter->rIcapInfo.au4Offset[1][0] = 0;
		prAdapter->rIcapInfo.au4Offset[1][1] = 9;
#endif /* CFG_SUPPORT_QA_TOOL */
		prAdapter->rIcapInfo.u2DumpIndex++;

	}
}


uint32_t nicRfTestEventHandler(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	uint32_t u4QueryInfoLen = 0;
	struct EXT_EVENT_RF_TEST_RESULT_T *prResult;
	struct EXT_EVENT_RBIST_CAP_STATUS_T *prCapStatus;
	struct mt66xx_chip_info *prChipInfo = NULL;
	struct ATE_OPS_T *prAteOps = NULL;

	ASSERT(prAdapter);
	prChipInfo = prAdapter->chip_info;
	ASSERT(prChipInfo);
	prAteOps = prChipInfo->prAteOps;
	ASSERT(prAteOps);

	prResult = (struct EXT_EVENT_RF_TEST_RESULT_T *)prEvent->aucBuffer;
	DBGLOG(RFTEST, INFO, "prResult->u4FuncIndex = %d\n", prResult->u4FuncIndex);
	switch (prResult->u4FuncIndex) {
	case GET_ICAP_CAPTURE_STATUS:
		u4QueryInfoLen = sizeof(struct EXT_EVENT_RBIST_CAP_STATUS_T);
		prCapStatus = (struct EXT_EVENT_RBIST_CAP_STATUS_T *)prEvent->aucBuffer;

		DBGLOG(RFTEST, INFO, "prCapStatus->u4CapDone = %d\n", prCapStatus->u4CapDone);
		if (prCapStatus->u4CapDone) {
			wlanoidRfTestICapRawDataProc(prAdapter,
								0 /*prCapStatus->u4CapStartAddr*/,
								0 /*prCapStatus->u4TotalBufferSize*/);
		}
		break;

	case GET_ICAP_RAW_DATA:
		if (prAteOps->getRbistDataDumpEvent)
			prAteOps->getRbistDataDumpEvent(prAdapter, prEvent->aucBuffer);
		break;

	default:
		DBGLOG(RFTEST, WARN, "Unknown rf test event, ignore\n");
		break;
	}

	return u4QueryInfoLen;
}

void nicEventLayer0ExtMagic(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	uint32_t u4QueryInfoLen = 0;
	struct CMD_INFO *prCmdInfo = NULL;
	struct EVENT_ACCESS_EFUSE *prEventEfuseAccess;
	struct EXT_EVENT_EFUSE_FREE_BLOCK *prEventGetFreeBlock;
	struct EXT_EVENT_GET_TX_POWER *prEventGetTXPower;

	switch (prEvent->ucExtenEID) {

	case EXT_EVENT_ID_CMD_RESULT:
		u4QueryInfoLen = sizeof(struct PARAM_CUSTOM_EFUSE_BUFFER_MODE);
		prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);
		break;

	case EXT_EVENT_ID_EFUSE_ACCESS:
		u4QueryInfoLen = sizeof(struct PARAM_CUSTOM_ACCESS_EFUSE);
		prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);
		prEventEfuseAccess = (struct EVENT_ACCESS_EFUSE *) (prEvent->aucBuffer);

		/* Efuse block size 16 */
		kalMemCopy(prAdapter->aucEepromVaule, prEventEfuseAccess->aucData, 16);
		break;

	case EXT_EVENT_ID_RF_TEST:
		u4QueryInfoLen = nicRfTestEventHandler(prAdapter, prEvent);
		prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);
		break;

	case EXT_EVENT_ID_GET_TX_POWER:
		u4QueryInfoLen = sizeof(struct PARAM_CUSTOM_GET_TX_POWER);
		prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);
		prEventGetTXPower = (struct EXT_EVENT_GET_TX_POWER *) (prEvent->aucBuffer);

		prAdapter->u4GetTxPower = prEventGetTXPower->ucTx0TargetPower;
		break;

	case EXT_EVENT_ID_EFUSE_FREE_BLOCK:
		u4QueryInfoLen = sizeof(struct PARAM_CUSTOM_EFUSE_FREE_BLOCK);
		prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);
		prEventGetFreeBlock = (struct EXT_EVENT_EFUSE_FREE_BLOCK *) (prEvent->aucBuffer);
		prAdapter->u4FreeBlockNum = prEventGetFreeBlock->u2FreeBlockNum;
		break;

	default:
		break;
	}

	if (prCmdInfo != NULL) {
		if ((prCmdInfo->fgIsOid) != 0) {
			kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery,
				u4QueryInfoLen, WLAN_STATUS_SUCCESS);
			/* return prCmdInfo */
			cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
		}
	}
#if (CFG_SUPPORT_TXPOWER_INFO == 1)
	else if ((prEvent->ucExtenEID) == EXT_EVENT_ID_TX_POWER_FEATURE_CTRL) {
		u4QueryInfoLen = sizeof(struct PARAM_TXPOWER_ALL_RATE_POWER_INFO_T);
		/* command response handling */
		prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

		if (prCmdInfo != NULL) {
			if (prCmdInfo->pfCmdDoneHandler)
				prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
			else if (prCmdInfo->fgIsOid)
				kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery,
								u4QueryInfoLen, WLAN_STATUS_SUCCESS);

			/* return prCmdInfo */
			cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
		}
	}
#endif
}

void nicEventMicErrorInfo(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct EVENT_MIC_ERR_INFO *prMicError;
	/* P_PARAM_AUTH_EVENT_T prAuthEvent; */
	struct STA_RECORD *prStaRec;

	DBGLOG(RSN, EVENT, "EVENT_ID_MIC_ERR_INFO\n");

	prMicError = (struct EVENT_MIC_ERR_INFO *) (prEvent->aucBuffer);
	prStaRec = cnmGetStaRecByAddress(prAdapter, prAdapter->prAisBssInfo->ucBssIndex,
					prAdapter->rWlanInfo.rCurrBssId.arMacAddress);
	ASSERT(prStaRec);

	if (prStaRec)
		rsnTkipHandleMICFailure(prAdapter, prStaRec, (u_int8_t) prMicError->u4Flags);
	else
		DBGLOG(RSN, INFO, "No STA rec!!\n");
#if 0
	prAuthEvent = (struct PARAM_AUTH_EVENT *) prAdapter->aucIndicationEventBuffer;

	/* Status type: Authentication Event */
	prAuthEvent->rStatus.eStatusType = ENUM_STATUS_TYPE_AUTHENTICATION;

	/* Authentication request */
	prAuthEvent->arRequest[0].u4Length = sizeof(struct PARAM_AUTH_REQUEST);
	kalMemCopy((void *) prAuthEvent->arRequest[0].arBssid,
			(void *) prAdapter->rWlanInfo.rCurrBssId.arMacAddress,	/* whsu:Todo? */
		   PARAM_MAC_ADDR_LEN);

	if (prMicError->u4Flags != 0)
		prAuthEvent->arRequest[0].u4Flags = PARAM_AUTH_REQUEST_GROUP_ERROR;
	else
		prAuthEvent->arRequest[0].u4Flags = PARAM_AUTH_REQUEST_PAIRWISE_ERROR;

	kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
				     WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
				     (void *) prAuthEvent,
				     sizeof(struct PARAM_STATUS_INDICATION) + sizeof(struct PARAM_AUTH_REQUEST));
#endif
}

void nicEventScanDone(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	scnEventScanDone(prAdapter, (struct EVENT_SCAN_DONE *) (prEvent->aucBuffer), TRUE);
}

void nicEventNloDone(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	DBGLOG(INIT, INFO, "EVENT_ID_NLO_DONE\n");
	scnEventNloDone(prAdapter, (struct EVENT_NLO_DONE *) (prEvent->aucBuffer));
#if CFG_SUPPORT_PNO
	prAdapter->prAisBssInfo->fgIsPNOEnable = FALSE;
	if (prAdapter->prAisBssInfo->fgIsNetRequestInActive && prAdapter->prAisBssInfo->fgIsPNOEnable) {
		UNSET_NET_ACTIVE(prAdapter, prAdapter->prAisBssInfo->ucBssIndex);
		DBGLOG(INIT, INFO, "INACTIVE  AIS from  ACTIVEto disable PNO\n");
		/* sync with firmware */
		nicDeactivateNetwork(prAdapter, prAdapter->prAisBssInfo->ucBssIndex);
	}
#endif
}

void nicEventSleepyNotify(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
#if !defined(_HIF_USB)
	struct EVENT_SLEEPY_INFO *prEventSleepyNotify;

	prEventSleepyNotify = (struct EVENT_SLEEPY_INFO *) (prEvent->aucBuffer);

	/* DBGLOG(RX, INFO, ("ucSleepyState = %d\n", prEventSleepyNotify->ucSleepyState)); */

	prAdapter->fgWiFiInSleepyState = (u_int8_t) (prEventSleepyNotify->ucSleepyState);

#if CFG_SUPPORT_MULTITHREAD
	if (prEventSleepyNotify->ucSleepyState)
		kalSetFwOwnEvent2Hif(prAdapter->prGlueInfo);
#endif
#endif
}

void nicEventBtOverWifi(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
#if CFG_ENABLE_BT_OVER_WIFI
	uint8_t aucTmp[sizeof(struct BT_OVER_WIFI_EVENT) + sizeof(struct BOW_LINK_DISCONNECTED)];
	struct EVENT_BT_OVER_WIFI *prEventBtOverWifi;
	struct BT_OVER_WIFI_EVENT *prBowEvent;
	struct BOW_LINK_CONNECTED *prBowLinkConnected;
	struct BOW_LINK_DISCONNECTED *prBowLinkDisconnected;

	prEventBtOverWifi = (struct EVENT_BT_OVER_WIFI *) (prEvent->aucBuffer);

	/* construct event header */
	prBowEvent = (struct BT_OVER_WIFI_EVENT *) aucTmp;

	if (prEventBtOverWifi->ucLinkStatus == 0) {
		/* Connection */
		prBowEvent->rHeader.ucEventId = BOW_EVENT_ID_LINK_CONNECTED;
		prBowEvent->rHeader.ucSeqNumber = 0;
		prBowEvent->rHeader.u2PayloadLength = sizeof(struct BOW_LINK_CONNECTED);

		/* fill event body */
		prBowLinkConnected = (struct BOW_LINK_CONNECTED *) (prBowEvent->aucPayload);
		prBowLinkConnected->rChannel.ucChannelNum = prEventBtOverWifi->ucSelectedChannel;
		kalMemZero(prBowLinkConnected->aucPeerAddress, MAC_ADDR_LEN);	/* @FIXME */

		kalIndicateBOWEvent(prAdapter->prGlueInfo, prBowEvent);
	} else {
		/* Disconnection */
		prBowEvent->rHeader.ucEventId = BOW_EVENT_ID_LINK_DISCONNECTED;
		prBowEvent->rHeader.ucSeqNumber = 0;
		prBowEvent->rHeader.u2PayloadLength = sizeof(struct BOW_LINK_DISCONNECTED);

		/* fill event body */
		prBowLinkDisconnected = (struct BOW_LINK_DISCONNECTED *) (prBowEvent->aucPayload);
		prBowLinkDisconnected->ucReason = 0;	/* @FIXME */
		kalMemZero(prBowLinkDisconnected->aucPeerAddress, MAC_ADDR_LEN);	/* @FIXME */

		kalIndicateBOWEvent(prAdapter->prGlueInfo, prBowEvent);
	}
#endif
}

void nicEventStatistics(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct CMD_INFO *prCmdInfo;

	/* buffer statistics for further query */
	prAdapter->fgIsStatValid = TRUE;
	prAdapter->rStatUpdateTime = kalGetTimeTick();
	kalMemCopy(&prAdapter->rStatStruct, prEvent->aucBuffer, sizeof(struct EVENT_STATISTICS));

	/* command response handling */
	prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

	if (prCmdInfo != NULL) {
		if (prCmdInfo->pfCmdDoneHandler)
			prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
		else if (prCmdInfo->fgIsOid)
			kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
		/* return prCmdInfo */
		cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
	}
}
void nicEventWlanInfo(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct CMD_INFO *prCmdInfo;

	/* buffer statistics for further query */
	prAdapter->fgIsStatValid = TRUE;
	prAdapter->rStatUpdateTime = kalGetTimeTick();
	kalMemCopy(&prAdapter->rEventWlanInfo, prEvent->aucBuffer, sizeof(struct EVENT_WLAN_INFO));

	DBGLOG(RSN, INFO, "EVENT_ID_WLAN_INFO");
	/* command response handling */
	prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

	if (prCmdInfo != NULL) {
		if (prCmdInfo->pfCmdDoneHandler)
			prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
		else if (prCmdInfo->fgIsOid)
			kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
		/* return prCmdInfo */
		cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
	}
}

void nicEventMibInfo(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct CMD_INFO *prCmdInfo;


	/* buffer statistics for further query */
	prAdapter->fgIsStatValid = TRUE;
	prAdapter->rStatUpdateTime = kalGetTimeTick();

	DBGLOG(RSN, INFO, "EVENT_ID_MIB_INFO");
	/* command response handling */
	prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

	if (prCmdInfo != NULL) {
		if (prCmdInfo->pfCmdDoneHandler)
			prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
		else if (prCmdInfo->fgIsOid)
			kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
		/* return prCmdInfo */
		cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
	}

}

void nicEventBeaconTimeout(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	DBGLOG(NIC, INFO, "EVENT_ID_BSS_BEACON_TIMEOUT\n");

	if (prAdapter->fgDisBcnLostDetection == FALSE) {
		struct BSS_INFO *prBssInfo = (struct BSS_INFO *) NULL;
		struct EVENT_BSS_BEACON_TIMEOUT *prEventBssBeaconTimeout;

		prEventBssBeaconTimeout = (struct EVENT_BSS_BEACON_TIMEOUT *) (prEvent->aucBuffer);

		if (prEventBssBeaconTimeout->ucBssIndex >= prAdapter->ucHwBssIdNum)
			return;

		DBGLOG(NIC, INFO, "Reason code: %d\n", prEventBssBeaconTimeout->ucReasonCode);

		prBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter, prEventBssBeaconTimeout->ucBssIndex);

		if ((prAdapter->prAisBssInfo != NULL) &&
		    (prEventBssBeaconTimeout->ucBssIndex ==
		     prAdapter->prAisBssInfo->ucBssIndex)) {
			aisBssBeaconTimeout(prAdapter);
		}
#if CFG_ENABLE_WIFI_DIRECT
		else if (prBssInfo->eNetworkType == NETWORK_TYPE_P2P)
			p2pRoleFsmRunEventBeaconTimeout(prAdapter, prBssInfo);
#endif
#if CFG_ENABLE_BT_OVER_WIFI
		else if (GET_BSS_INFO_BY_INDEX(prAdapter, prEventBssBeaconTimeout->ucBssIndex)->eNetworkType ==
			 NETWORK_TYPE_BOW) {
			/* ToDo:: Nothing */
		}
#endif
		else {
			DBGLOG(RX, ERROR, "EVENT_ID_BSS_BEACON_TIMEOUT: (ucBssIndex = %d)\n",
				prEventBssBeaconTimeout->ucBssIndex);
		}
	}

}

void nicEventUpdateNoaParams(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
#if CFG_ENABLE_WIFI_DIRECT
	if (prAdapter->fgIsP2PRegistered) {
		struct EVENT_UPDATE_NOA_PARAMS *prEventUpdateNoaParam;

		prEventUpdateNoaParam = (struct EVENT_UPDATE_NOA_PARAMS *) (prEvent->aucBuffer);

		if (GET_BSS_INFO_BY_INDEX(prAdapter, prEventUpdateNoaParam->ucBssIndex)->eNetworkType ==
			NETWORK_TYPE_P2P) {

			p2pProcessEvent_UpdateNOAParam(prAdapter, prEventUpdateNoaParam->ucBssIndex,
				prEventUpdateNoaParam);
		} else {
			ASSERT(0);
		}
	}
#else
	ASSERT(0);
#endif
}

void nicEventStaAgingTimeout(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	if (prAdapter->fgDisStaAgingTimeoutDetection == FALSE) {
		struct EVENT_STA_AGING_TIMEOUT *prEventStaAgingTimeout;
		struct STA_RECORD *prStaRec;
		struct BSS_INFO *prBssInfo = (struct BSS_INFO *) NULL;

		prEventStaAgingTimeout = (struct EVENT_STA_AGING_TIMEOUT *) (prEvent->aucBuffer);
		prStaRec = cnmGetStaRecByIndex(prAdapter, prEventStaAgingTimeout->ucStaRecIdx);
		if (prStaRec == NULL)
			return;

		DBGLOG(NIC, INFO, "EVENT_ID_STA_AGING_TIMEOUT: STA[%u] " MACSTR "\n",
			prEventStaAgingTimeout->ucStaRecIdx, MAC2STR(prStaRec->aucMacAddr));

		prBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter, prStaRec->ucBssIndex);

		bssRemoveClient(prAdapter, prBssInfo, prStaRec);

		if (prAdapter->fgIsP2PRegistered) {
			p2pFuncDisconnect(prAdapter, prBssInfo, prStaRec, FALSE,
				REASON_CODE_DISASSOC_INACTIVITY);
		}

	}
	/* gDisStaAgingTimeoutDetection */
}

void nicEventApObssStatus(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
#if CFG_ENABLE_WIFI_DIRECT
	if (prAdapter->fgIsP2PRegistered)
		rlmHandleObssStatusEventPkt(prAdapter, (struct EVENT_AP_OBSS_STATUS *) prEvent->aucBuffer);
#endif
}

void nicEventRoamingStatus(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
#if CFG_SUPPORT_ROAMING
	struct CMD_ROAMING_TRANSIT *prTransit;

	prTransit = (struct CMD_ROAMING_TRANSIT *) (prEvent->aucBuffer);

	roamingFsmProcessEvent(prAdapter, prTransit);
#endif /* CFG_SUPPORT_ROAMING */
}

void nicEventSendDeauth(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct SW_RFB rSwRfb;

	DBGLOG(NIC, INFO, "%s\n", __func__);
#if DBG
	struct WLAN_MAC_HEADER *prWlanMacHeader;

	prWlanMacHeader = (struct WLAN_MAC_HEADER *) &prEvent->aucBuffer[0];
	DBGLOG(RX, TRACE, "nicRx: aucAddr1: " MACSTR "\n", MAC2STR(prWlanMacHeader->aucAddr1));
	DBGLOG(RX, TRACE, "nicRx: aucAddr2: " MACSTR "\n", MAC2STR(prWlanMacHeader->aucAddr2));
#endif

	/* receive packets without StaRec */
	rSwRfb.pvHeader = (struct WLAN_MAC_HEADER *) &prEvent->aucBuffer[0];
	if (authSendDeauthFrame(prAdapter, NULL, NULL, &rSwRfb, REASON_CODE_CLASS_3_ERR,
		(PFN_TX_DONE_HANDLER) NULL) == WLAN_STATUS_SUCCESS) {

		DBGLOG(RX, ERROR, "Send Deauth Error\n");
	}
}

void nicEventUpdateRddStatus(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
#if CFG_SUPPORT_RDD_TEST_MODE
	struct EVENT_RDD_STATUS *prEventRddStatus;

	prEventRddStatus = (struct EVENT_RDD_STATUS *) (prEvent->aucBuffer);

	prAdapter->ucRddStatus = prEventRddStatus->ucRddStatus;
#endif
}

void nicEventUpdateBwcsStatus(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct PTA_IPC *prEventBwcsStatus;

	prEventBwcsStatus = (struct PTA_IPC *) (prEvent->aucBuffer);

#if CFG_SUPPORT_BCM_BWCS_DEBUG
	DBGLOG(RSN, EVENT, "BCM BWCS Event: %02x%02x%02x%02x\n",
	       prEventBwcsStatus->u.aucBTPParams[0],
	       prEventBwcsStatus->u.aucBTPParams[1],
	       prEventBwcsStatus->u.aucBTPParams[2], prEventBwcsStatus->u.aucBTPParams[3]);
#endif

	kalIndicateStatusAndComplete(prAdapter->prGlueInfo, WLAN_STATUS_BWCS_UPDATE,
		(void *) prEventBwcsStatus, sizeof(struct PTA_IPC));
}

void nicEventUpdateBcmDebug(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct PTA_IPC *prEventBwcsStatus;

	prEventBwcsStatus = (struct PTA_IPC *) (prEvent->aucBuffer);

#if CFG_SUPPORT_BCM_BWCS_DEBUG
	DBGLOG(RSN, EVENT, "BCM FW status: %02x%02x%02x%02x\n",
	       prEventBwcsStatus->u.aucBTPParams[0],
	       prEventBwcsStatus->u.aucBTPParams[1],
	       prEventBwcsStatus->u.aucBTPParams[2], prEventBwcsStatus->u.aucBTPParams[3]);
#endif
}

void nicEventAddPkeyDone(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct EVENT_ADD_KEY_DONE_INFO *prKeyDone;
	struct STA_RECORD *prStaRec = NULL;
	uint8_t ucKeyId;

	prKeyDone = (struct EVENT_ADD_KEY_DONE_INFO *) (prEvent->aucBuffer);

	DBGLOG(RSN, INFO, "EVENT_ID_ADD_PKEY_DONE BSSIDX=%d " MACSTR "\n",
		prKeyDone->ucBSSIndex, MAC2STR(prKeyDone->aucStaAddr));

	prStaRec = cnmGetStaRecByAddress(prAdapter, prKeyDone->ucBSSIndex, prKeyDone->aucStaAddr);

	if (!prStaRec) {
		ucKeyId = prAdapter->rWifiVar.rAisSpecificBssInfo.ucKeyAlgorithmId;
		if ((ucKeyId == CIPHER_SUITE_WEP40) || (ucKeyId == CIPHER_SUITE_WEP104)) {
			DBGLOG(RX, INFO, "WEP, ucKeyAlgorithmId= %d\n", ucKeyId);
			prStaRec = cnmGetStaRecByAddress(prAdapter, prKeyDone->ucBSSIndex,
				prAdapter->rWifiVar.arBssInfoPool[prKeyDone->ucBSSIndex].aucBSSID);
			if (!prStaRec) {
				DBGLOG(RX, INFO, "WEP, AddPKeyDone, ucBSSIndex %d, Addr %pM, StaRec is NULL\n",
					prKeyDone->ucBSSIndex,
					prAdapter->rWifiVar.arBssInfoPool[prKeyDone->ucBSSIndex].aucBSSID);
			}
		} else {
			DBGLOG(RX, INFO, "AddPKeyDone, ucBSSIndex %d, Addr %pM, StaRec is NULL\n",
				prKeyDone->ucBSSIndex, prKeyDone->aucStaAddr);
		}
	}
	if (prStaRec) {
		DBGLOG(RSN, INFO, "STA " MACSTR " Add Key Done!!\n", MAC2STR(prStaRec->aucMacAddr));
		prStaRec->fgIsTxKeyReady = TRUE;
		qmUpdateStaRec(prAdapter, prStaRec);
	}
}

void nicEventIcapDone(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct EVENT_ICAP_STATUS *prEventIcapStatus;
	struct PARAM_CUSTOM_MEM_DUMP_STRUCT rMemDumpInfo;
	uint32_t u4QueryInfo;

	prEventIcapStatus = (struct EVENT_ICAP_STATUS *) (prEvent->aucBuffer);

	rMemDumpInfo.u4Address = prEventIcapStatus->u4StartAddress;
	rMemDumpInfo.u4Length = prEventIcapStatus->u4IcapSieze;
#if CFG_SUPPORT_QA_TOOL
	rMemDumpInfo.u4IcapContent = prEventIcapStatus->u4IcapContent;
#endif

	wlanoidQueryMemDump(prAdapter, &rMemDumpInfo, sizeof(rMemDumpInfo), &u4QueryInfo);
}

#if CFG_SUPPORT_CAL_RESULT_BACKUP_TO_HOST
struct PARAM_CAL_BACKUP_STRUCT_V2	g_rCalBackupDataV2;

void nicEventCalAllDone(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct CMD_CAL_BACKUP_STRUCT_V2 *prEventCalBackupDataV2;
	uint32_t u4QueryInfo;

	DBGLOG(RFTEST, INFO, "%s\n", __func__);

	memset(&g_rCalBackupDataV2, 0, sizeof(struct PARAM_CAL_BACKUP_STRUCT_V2));

	prEventCalBackupDataV2 = (struct CMD_CAL_BACKUP_STRUCT_V2 *) (prEvent->aucBuffer);

	if (prEventCalBackupDataV2->ucReason == 1 && prEventCalBackupDataV2->ucAction == 2) {
		DBGLOG(RFTEST, INFO, "Received an EVENT for Trigger Do All Cal Function.\n");

		g_rCalBackupDataV2.ucReason = 2;
		g_rCalBackupDataV2.ucAction = 4;
		g_rCalBackupDataV2.ucNeedResp = 1;
		g_rCalBackupDataV2.ucFragNum = 0;
		g_rCalBackupDataV2.ucRomRam = 0;
		g_rCalBackupDataV2.u4ThermalValue = 0;
		g_rCalBackupDataV2.u4Address = 0;
		g_rCalBackupDataV2.u4Length = 0;
		g_rCalBackupDataV2.u4RemainLength = 0;

		DBGLOG(RFTEST, INFO, "RLM CMD : Get Cal Data from FW (%s). Start!!!!!!!!!!!!!!!!\n",
			g_rCalBackupDataV2.ucRomRam == 0 ? "ROM" : "RAM");
		DBGLOG(RFTEST, INFO, "Thermal Temp = %d\n", g_rBackupCalDataAllV2.u4ThermalInfo);
		wlanoidQueryCalBackupV2(prAdapter,
			&g_rCalBackupDataV2,
			sizeof(struct PARAM_CAL_BACKUP_STRUCT_V2),
			&u4QueryInfo);
	}

}
#endif

void nicEventDebugMsg(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct EVENT_DEBUG_MSG *prEventDebugMsg;
	uint8_t ucMsgType;
	uint16_t u2MsgSize;
	uint8_t *pucMsg;

	prEventDebugMsg = (struct EVENT_DEBUG_MSG *)(prEvent->aucBuffer);
	ucMsgType = prEventDebugMsg->ucMsgType;
	u2MsgSize = prEventDebugMsg->u2MsgSize;
	pucMsg = prEventDebugMsg->aucMsg;

	wlanPrintFwLog(pucMsg, u2MsgSize, ucMsgType, NULL);
}

void nicEventTdls(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	TdlsexEventHandle(prAdapter->prGlueInfo, (uint8_t *)prEvent->aucBuffer,
		(uint32_t)(prEvent->u2PacketLength - 8));
}

void nicEventDumpMem(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct CMD_INFO *prCmdInfo;

	DBGLOG(SW4, INFO, "%s: EVENT_ID_DUMP_MEM\n", __func__);

	prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

	if (prCmdInfo != NULL) {
		DBGLOG(NIC, INFO, ": ==> 1\n");
		if (prCmdInfo->pfCmdDoneHandler)
			prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
		else if (prCmdInfo->fgIsOid)
			kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
		/* return prCmdInfo */
		cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
	} else {
		/* Burst mode */
		DBGLOG(NIC, INFO, ": ==> 2\n");
#if 0
		nicEventQueryMemDump(prAdapter, prEvent->aucBuffer);
#endif
	}
}

void nicEventAssertDump(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{

	if (wlanIsChipRstRecEnabled(prAdapter))
		wlanChipRstPreAct(prAdapter);

	if (prEvent->ucS2DIndex == S2D_INDEX_EVENT_N2H) {
		if (!prAdapter->fgN9AssertDumpOngoing) {
			DBGLOG(NIC, ERROR, "%s: EVENT_ID_ASSERT_DUMP\n", __func__);
			DBGLOG(NIC, ERROR, "\n[DUMP_N9]====N9 ASSERT_DUMPSTART====\n");
			prAdapter->fgKeepPrintCoreDump = TRUE;
			if (kalOpenCorDumpFile(TRUE) != WLAN_STATUS_SUCCESS)
				DBGLOG(NIC, ERROR, "kalOpenCorDumpFile fail\n");
			else
				prAdapter->fgN9CorDumpFileOpend = TRUE;

			prAdapter->fgN9AssertDumpOngoing = TRUE;
				}
		if (prAdapter->fgN9AssertDumpOngoing) {

			if (prAdapter->fgKeepPrintCoreDump)
				DBGLOG(NIC, ERROR, "[DUMP_N9]%s:\n", prEvent->aucBuffer);
			if (!kalStrnCmp(prEvent->aucBuffer, ";more log added here", 5)
			    || !kalStrnCmp(prEvent->aucBuffer, ";[core dump start]", 5))
				prAdapter->fgKeepPrintCoreDump = FALSE;

			if (prAdapter->fgN9CorDumpFileOpend) {
				if (kalWriteCorDumpFile(prEvent->aucBuffer,
						prEvent->u2PacketLength - EVENT_HDR_WITHOUT_RXD_SIZE, TRUE) !=
						WLAN_STATUS_SUCCESS) {
					DBGLOG(NIC, INFO, "kalWriteN9CorDumpFile fail\n");
				}
			}
			wlanCorDumpTimerReset(prAdapter, TRUE);
		}
	} else {
		/* prEvent->ucS2DIndex == S2D_INDEX_EVENT_C2H */
		if (!prAdapter->fgCr4AssertDumpOngoing) {
			DBGLOG(NIC, ERROR, "%s: EVENT_ID_ASSERT_DUMP\n", __func__);
			DBGLOG(NIC, ERROR, "\n[DUMP_Cr4]====CR4 ASSERT_DUMPSTART====\n");
			prAdapter->fgKeepPrintCoreDump = TRUE;
			if (kalOpenCorDumpFile(FALSE) != WLAN_STATUS_SUCCESS)
				DBGLOG(NIC, ERROR, "kalOpenCorDumpFile fail\n");
			else
				prAdapter->fgCr4CorDumpFileOpend = TRUE;

			prAdapter->fgCr4AssertDumpOngoing = TRUE;
		}
		if (prAdapter->fgCr4AssertDumpOngoing) {
			if (prAdapter->fgKeepPrintCoreDump)
				DBGLOG(NIC, ERROR, "[DUMP_CR4]%s:\n", prEvent->aucBuffer);
			if (!kalStrnCmp(prEvent->aucBuffer, ";more log added here", 5))
				prAdapter->fgKeepPrintCoreDump = FALSE;

			if (prAdapter->fgCr4CorDumpFileOpend) {
				if (kalWriteCorDumpFile(prEvent->aucBuffer,
					prEvent->u2PacketLength - EVENT_HDR_WITHOUT_RXD_SIZE, FALSE) !=
					WLAN_STATUS_SUCCESS) {
					DBGLOG(NIC, ERROR, "kalWriteN9CorDumpFile fail\n");
				}
			}
			wlanCorDumpTimerReset(prAdapter, FALSE);
		}
	}
}

void nicEventRddSendPulse(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	DBGLOG(RLM, INFO, "%s: EVENT_ID_RDD_SEND_PULSE\n", __func__);

	nicEventRddPulseDump(prAdapter, prEvent->aucBuffer);
}

void nicEventUpdateCoexPhyrate(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	uint8_t i;
	struct EVENT_UPDATE_COEX_PHYRATE *prEventUpdateCoexPhyrate;

	ASSERT(prAdapter);

	DBGLOG(NIC, LOUD, "%s\n", __func__);

	prEventUpdateCoexPhyrate = (struct EVENT_UPDATE_COEX_PHYRATE *)(prEvent->aucBuffer);

	for (i = 0; i < (prAdapter->ucHwBssIdNum + 1); i++) {
		prAdapter->aprBssInfo[i]->u4CoexPhyRateLimit = prEventUpdateCoexPhyrate->au4PhyRateLimit[i];
		DBGLOG(NIC, INFO, "Coex:BSS[%d]R:%d\n", i, prAdapter->aprBssInfo[i]->u4CoexPhyRateLimit);
	}

	prAdapter->ucSmarGearSupportSisoOnly = prEventUpdateCoexPhyrate->ucSupportSisoOnly;
	prAdapter->ucSmartGearWfPathSupport = prEventUpdateCoexPhyrate->ucWfPathSupport;

	DBGLOG(NIC, INFO, "Smart Gear SISO:%d, WF:%d\n",
		prAdapter->ucSmarGearSupportSisoOnly, prAdapter->ucSmartGearWfPathSupport);
}

void nicCmdEventQueryCnmInfo(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct PARAM_GET_CNM_T *prCnmInfoQuery = NULL;
	struct PARAM_GET_CNM_T *prCnmInfoEvent = NULL;
	struct GLUE_INFO *prGlueInfo;
	uint32_t u4QueryInfoLen;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(pucEventBuf);

	if (prCmdInfo->fgIsOid) {
		prCnmInfoQuery = (struct PARAM_GET_CNM_T *)prCmdInfo->pvInformationBuffer;
		prCnmInfoEvent = (struct PARAM_GET_CNM_T *)pucEventBuf;
		kalMemCopy(prCnmInfoQuery, prCnmInfoEvent, sizeof(struct PARAM_GET_CNM_T));

		prGlueInfo = prAdapter->prGlueInfo;
		u4QueryInfoLen = sizeof(struct PARAM_GET_CNM_T);
		kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
	}
}

void nicEventCnmInfo(IN struct ADAPTER *prAdapter, IN struct WIFI_EVENT *prEvent)
{
	struct CMD_INFO *prCmdInfo;

	/* command response handling */
	prCmdInfo = nicGetPendingCmdInfo(prAdapter, prEvent->ucSeqNum);

	if (prCmdInfo != NULL) {
		if (prCmdInfo->pfCmdDoneHandler)
			prCmdInfo->pfCmdDoneHandler(prAdapter, prCmdInfo, prEvent->aucBuffer);
		else if (prCmdInfo->fgIsOid)
			kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_SUCCESS);
		/* return prCmdInfo */
		cmdBufFreeCmdInfo(prAdapter, prCmdInfo);
	}
}

#if CFG_SUPPORT_REPLAY_DETECTION
void nicCmdEventSetAddKey(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo, IN uint8_t *pucEventBuf)
{
	struct WIFI_CMD *prWifiCmd = NULL;
	struct CMD_802_11_KEY *prCmdKey = NULL;
	struct GLUE_INFO *prGlueInfo = NULL;
	struct GL_DETECT_REPLAY_INFO *prDetRplyInfo = NULL;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	if (prCmdInfo->fgIsOid) {
		/* Update Set Information Length */
		kalOidComplete(prAdapter->prGlueInfo,
			       prCmdInfo->fgSetQuery, prCmdInfo->u4InformationBufferLength, WLAN_STATUS_SUCCESS);
	}

	prGlueInfo = prAdapter->prGlueInfo;
	prDetRplyInfo = &prGlueInfo->prDetRplyInfo;
	if (pucEventBuf) {
		prWifiCmd = (struct WIFI_CMD *) (pucEventBuf);
		prCmdKey = (struct CMD_802_11_KEY *) (prWifiCmd->aucBuffer);
		if (!prCmdKey->ucKeyType) {
			prDetRplyInfo->ucCurKeyId = prCmdKey->ucKeyId;
			prDetRplyInfo->ucKeyType = prCmdKey->ucKeyType;
			prDetRplyInfo->arReplayPNInfo[prCmdKey->ucKeyId].fgRekey = TRUE;
			prDetRplyInfo->arReplayPNInfo[prCmdKey->ucKeyId].fgFirstPkt = TRUE;
			DBGLOG(NIC, TRACE, "Keyid is %d, ucKeyType is %d\n",
				prCmdKey->ucKeyId, prCmdKey->ucKeyType);
		}
	}
}
void nicOidCmdTimeoutSetAddKey(IN struct ADAPTER *prAdapter, IN struct CMD_INFO *prCmdInfo)
{
	ASSERT(prAdapter);

	DBGLOG(NIC, WARN, "Wlan setaddkey timeout.\n");
	if (prCmdInfo->fgIsOid)
		kalOidComplete(prAdapter->prGlueInfo, prCmdInfo->fgSetQuery, 0, WLAN_STATUS_FAILURE);
}
#endif

