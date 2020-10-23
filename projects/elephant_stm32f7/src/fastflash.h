#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>

#define __ASM                   __asm
#define __STATIC_FORCEINLINE    __attribute__((always_inline)) static inline

/**
  \brief   Instruction Synchronization Barrier
  \details Instruction Synchronization Barrier flushes the pipeline in the processor,
           so that all instructions following the ISB are fetched from cache or memory,
           after the instruction has been completed.
 */
__STATIC_FORCEINLINE void __ISB(void)
{
  __ASM volatile ("isb 0xF":::"memory");
}

#define SCB_ICIALLU  MMIO32(0xE000EF50)
/**
  \brief   Data Synchronization Barrier
  \details Acts as a special kind of Data Memory Barrier.
           It completes when all explicit memory accesses before this instruction complete.
 */
__STATIC_FORCEINLINE void __DSB(void)
{
  __ASM volatile ("dsb 0xF":::"memory");
}

#define SCB_CCR_IC_Pos                      17U                                           /*!< SCB CCR: Instruction cache enable bit Position */
#define SCB_CCR_IC_Msk                     (1UL << SCB_CCR_IC_Pos)                        /*!< SCB CCR: Instruction cache enable bit Mask */
__STATIC_FORCEINLINE void SCB_EnableICache (void)
{
    if (SCB_CCR & SCB_CCR_IC_Msk) return;  /* return if ICache is already enabled */

    __DSB();
    __ISB();
    SCB_ICIALLU = 0UL;                     /* invalidate I-Cache */
    __DSB();
    __ISB();
    SCB_CCR |=  (uint32_t)SCB_CCR_IC_Msk;  /* enable I-Cache */
    __DSB();
    __ISB();
}

#define SCB_CCR_DC_Pos                      16U                                           /*!< SCB CCR: Cache enable bit Position */
#define SCB_CCR_DC_Msk                     (1UL << SCB_CCR_DC_Pos)                        /*!< SCB CCR: Cache enable bit Mask */
#define SCB_CSSELR  MMIO32(0xE000ED84)
#define SCB_CCSIDR  MMIO32(0xE000ED80)
#define SCB_DCISW   MMIO32(0xE000EF60)
//#define SCB_CCR     MMIO32(0xE000ED14)        //Already defined


#define SCB_CCSIDR_ASSOCIATIVITY_Pos        3U                                            /*!< SCB CCSIDR: Associativity Position */
#define SCB_CCSIDR_ASSOCIATIVITY_Msk       (0x3FFUL << SCB_CCSIDR_ASSOCIATIVITY_Pos)      /*!< SCB CCSIDR: Associativity Mask */
#define SCB_CCSIDR_NUMSETS_Pos             13U                                            /*!< SCB CCSIDR: NumSets Position */
#define SCB_CCSIDR_NUMSETS_Msk             (0x7FFFUL << SCB_CCSIDR_NUMSETS_Pos)           /*!< SCB CCSIDR: NumSets Mask */
#define CCSIDR_WAYS(x)         (((x) & SCB_CCSIDR_ASSOCIATIVITY_Msk) >> SCB_CCSIDR_ASSOCIATIVITY_Pos)
#define CCSIDR_SETS(x)         (((x) & SCB_CCSIDR_NUMSETS_Msk      ) >> SCB_CCSIDR_NUMSETS_Pos      )
#define __SCB_DCACHE_LINE_SIZE  32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */


#define SCB_DCISW_SET_Pos                   5U                                            /*!< SCB DCISW: Set Position */
#define SCB_DCISW_SET_Msk                  (0x1FFUL << SCB_DCISW_SET_Pos)                 /*!< SCB DCISW: Set Mask */
/* SCB D-Cache Clean by Set-way Register Definitions */
#define SCB_DCISW_WAY_Pos                  30U                                            /*!< SCB DCISW: Way Position */
#define SCB_DCISW_WAY_Msk                  (3UL << SCB_DCISW_WAY_Pos)                     /*!< SCB DCISW: Way Mask */


/**
  \brief   Enable D-Cache
  \details Turns on D-Cache
  */
__STATIC_FORCEINLINE void SCB_EnableDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    if (SCB_CCR & SCB_CCR_DC_Msk) return;  /* return if DCache is already enabled */

    SCB_CSSELR = 0U;                       /* select Level 1 data cache */
    __DSB();

    ccsidr = SCB_CCSIDR;

                                            /* invalidate D-Cache */
    sets = (uint32_t)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
      do {
        SCB_DCISW = (((sets << SCB_DCISW_SET_Pos) & SCB_DCISW_SET_Msk) |
                      ((ways << SCB_DCISW_WAY_Pos) & SCB_DCISW_WAY_Msk)  );
        //#if defined ( __CC_ARM )
        //  __schedule_barrier();
        //#endif
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    __DSB();

    SCB_CCR |=  (uint32_t)SCB_CCR_DC_Msk;  /* enable D-Cache */

    __DSB();
    __ISB();
}


#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define __HAL_FLASH_ART_ENABLE()  SET_BIT(FLASH_ACR, FLASH_ACR_ARTEN)
#define __HAL_FLASH_PREFETCH_BUFFER_ENABLE()  (FLASH_ACR |= FLASH_ACR_PRFTEN)

void enable_fastflash()
{
    SCB_EnableICache();
    SCB_EnableDCache();
    __HAL_FLASH_ART_ENABLE();
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
}
