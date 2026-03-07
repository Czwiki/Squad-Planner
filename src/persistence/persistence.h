/**
 * @file persistence.h
 * @brief Header file for persistence (save/load) functions.
 *
 * Provides the interface for saving application state to JSON files
 * and loading it back. The module serializes all players and formations
 * including their position assignments and player preference orders.
 *
 * File format: JSON via the cJSON library (MIT licensed, vendored in lib/cJSON/).
 * Default save location: "squad_planner_data.json" in the working directory.
 */

#ifndef PERSISTENCE_H
#define PERSISTENCE_H

/** @brief Default filename for save data */
#define SP_DEFAULT_SAVE_FILE "squad_planner_data"

/**
 * @brief Save all application data (players, formations) to a JSON file.
 *
 * Serializes the global player list and formation list into a JSON structure
 * and writes it to the specified file. Existing files are overwritten.
 *
 * @param filename Path to the output file (NULL uses SP_DEFAULT_SAVE_FILE)
 * @return SP_SUCCESS on success, SP_ERR_FILE_WRITE on write failure,
 *         SP_ERR_FILE_OPEN on file open failure, SP_ERR_MEMORY on allocation error
 */
int save_to_file(const char* filename);

/**
 * @brief Load all application data from a JSON file.
 *
 * Reads and parses a JSON file, then recreates all players and formations
 * in memory. Existing in-memory data is cleared before loading.
 *
 * @param filename Path to the input file (NULL uses SP_DEFAULT_SAVE_FILE)
 * @return SP_SUCCESS on success, SP_ERR_FILE_READ on read failure,
 *         SP_ERR_FILE_OPEN on file open failure, SP_ERR_FILE_FORMAT on parse error
 */
int load_from_file(const char* filename);

#endif /* PERSISTENCE_H */
