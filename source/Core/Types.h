/**
 * @file Types.h
 * @brief Core type definitions for MentalEngine
 * @author MentalEngine Team
 * @version 1.0.0
 * @date 2024
 * 
 * This file contains fundamental type definitions used throughout the MentalEngine project.
 * It provides convenient aliases and macros for common types to improve code readability
 * and maintainability.
 */

#ifndef MENTAL_TYPES_H
#define MENTAL_TYPES_H

/**
 * @def nothing
 * @brief Macro that expands to nullptr
 * 
 * This macro provides a more readable alternative to nullptr for cases where
 * a pointer should explicitly represent "nothing" or "no value".
 */
#define nothing nullptr

/**
 * @typedef nil
 * @brief Type alias for void
 * 
 * This typedef provides a more concise way to represent void type,
 * commonly used for functions that don't return a value or for
 * generic pointer types in the MentalEngine codebase.
 */
typedef void nil;

#endif // MENTAL_TYPES_H
