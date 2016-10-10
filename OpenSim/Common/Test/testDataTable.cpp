/* -------------------------------------------------------------------------- *
 *                            OpenSim:  testDataTable.cpp                     *
 * -------------------------------------------------------------------------- *
 * The OpenSim API is a toolkit for musculoskeletal modeling and simulation.  *
 * See http://opensim.stanford.edu and the NOTICE file for more information.  *
 * OpenSim is developed at Stanford University and supported by the US        *
 * National Institutes of Health (U54 GM072970, R24 HD065690) and by DARPA    *
 * through the Warrior Web program.                                           *
 *                                                                            *
 * Copyright (c) 2005-2016 Stanford University and the Authors                *
 * Authors:                                                                   *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */
#include <OpenSim/Auxiliary/auxiliaryTestFunctions.h>
#include <OpenSim/Common/TimeSeriesTable.h>
#include <iostream>

int main() {
    using namespace SimTK;
    using namespace OpenSim;
    using OpenSim::Exception;

    // Default construct, add metadata to columns, append rows one at a time.

    ValueArray<std::string> labels{};
    for(unsigned i = 1; i <= 5; ++i)
        labels.upd().push_back(SimTK::Value<std::string>{std::to_string(i)});

    ValueArray<unsigned> col_index{};
    for(unsigned i = 1; i <= 5; ++i)
        col_index.upd().push_back(SimTK::Value<unsigned>{i});

    DataTable::DependentsMetaData dep_metadata{};
    dep_metadata.setValueArrayForKey("labels", labels);
    dep_metadata.setValueArrayForKey("column-index", col_index);

    DataTable::IndependentMetaData ind_metadata{};
    ind_metadata.setValueForKey("labels", std::string{"0"});
    ind_metadata.setValueForKey("column-index", unsigned{0});

    TimeSeriesTable table{};
    {
        ASSERT(!table.hasColumnLabels());
        table.setColumnLabels({"0", "1", "2", "3"});
        ASSERT(table.hasColumnLabels());
        assert(table.hasColumn("1"));
        assert(table.hasColumn("2"));
        assert(!table.hasColumn("column-does-not-exist"));

        table.setColumnLabel(0, "zero");
        table.setColumnLabel(2, "two");
        
        assert(table.getColumnLabel(0) == "zero");
        assert(table.getColumnLabel(2) == "two");

        table.setColumnLabel(0, "0");
        table.setColumnLabel(2, "2");

        const auto& labels = table.getColumnLabels();
        for(size_t i = 0; i < labels.size(); ++i) 
            if(labels.at(i) != std::to_string(i))
                throw Exception{"Test failed: labels.at(i) != "
                                "std::to_string(i)"};

        for(size_t i = 0; i < labels.size(); ++i)
            if(table.getColumnIndex(labels.at(i)) != i)
                throw Exception{"Test failed: "
                                "table.getColumnIndex(labels.at(i)) != i"};
    }
    // Print out the DataTable to console.
    std::cout << table << std::endl;

    table.setDependentsMetaData(dep_metadata);
    table.setIndependentMetaData(ind_metadata);

    SimTK::RowVector_<double> row{5, double{0}};

    for(unsigned i = 0; i < 5; ++i)
        table.appendRow(0.00 + 0.25 * i, row + i);

    for(unsigned i = 0; i < 5; ++i)
        table.updRowAtIndex(i) += 1;

    for(unsigned i = 0; i < 5; ++i)
        table.updRow(0 + 0.25 * i) -= 1;

    try {
        table.appendRow(0.5, row);
    } catch (OpenSim::Exception&) {}

    table.updMatrix() += 2;
    table.updMatrixBlock(0, 0, table.getNumRows(), table.getNumColumns()) -= 2;

    table.updTableMetaData().setValueForKey("DataRate", 600);
    table.updTableMetaData().setValueForKey("Filename", 
                                            std::string{"/path/to/file"});

    assert(table.hasColumn(0));
    assert(table.hasColumn(2));
    assert(!table.hasColumn(100));

    // Print out the DataTable to console.
    std::cout << table << std::endl;

    // Retrieve added metadata and rows to check.
    if(table.getNumRows() != unsigned{5})
        throw Exception{"Test Failed: table.getNumRows() != unsigned{5}"};

    if(table.getNumColumns() != unsigned{5})
        throw Exception{"Test Failed: table.getNumColumns() != unsigned{5}"};

    const auto& dep_metadata_ref = table.getDependentsMetaData();

    const auto& labels_ref = dep_metadata_ref.getValueArrayForKey("labels");
    for(unsigned i = 0; i < 5; ++i)
        if(labels_ref[i].getValue<std::string>() != std::to_string(i + 1))
            throw Exception{"Test failed: labels_ref[i].getValue<std::string>()"
                    " != std::to_string(i + 1)"};
    {
    const auto& labels = table.getColumnLabels();
    for(unsigned i = 0; i < 5; ++i)
        if(labels.at(i) != std::to_string(i + 1))
            throw Exception{"Test failed: labels[i].getValue<std::string>()"
                    " != std::to_string(i + 1)"};
    }

    const auto& col_index_ref 
        = dep_metadata_ref.getValueArrayForKey("column-index");
    for(unsigned i = 0; i < 5; ++i)
        if(col_index_ref[i].getValue<unsigned>() != i + 1)
            throw Exception{"Test failed: col_index_ref[i].getValue<unsigned>()"
                    " != i + 1"};

    const auto& ind_metadata_ref = table.getIndependentMetaData();
    
    if(ind_metadata_ref.getValueForKey("labels").getValue<std::string>() 
       != std::string{"0"})
        throw Exception{"Test failed: ind_metadata_ref.getValueForKey"
                "(\"labels\").getValue<std::string>() != std::string{\"0\"}"};
    if(ind_metadata_ref.getValueForKey("column-index").getValue<unsigned>()
       != unsigned{0})
        throw Exception{"Test failed: ind_metadata_ref.getValueForKey"
                "(\"column-index\").getValue<unsigned>() != unsigned{0}"};

    table.updDependentColumnAtIndex(0) += 2;
    table.updDependentColumnAtIndex(2) += 2;
    table.updDependentColumn("1") -= 2;
    table.updDependentColumn("3") -= 2;

    for(unsigned i = 0; i < 5; ++i) {
        for(unsigned j = 0; j < 5; ++j) {
            const auto row_i_1 = table.getRowAtIndex(i);
            if(row_i_1[j] != (row + i)[j])
                throw Exception{"Test failed: row_i_1[j] != (row + i)[j]"};

            const auto row_i_2 = table.getRow(0 + 0.25 * i);
            if(row_i_2[j] != (row + i)[j])
                throw Exception{"Test failed: row_i_2[j] != (row + i)[j]"};

            const auto col_i = table.getDependentColumnAtIndex(i);
            if(col_i[j] != j)
                throw Exception{"Test failed: table.getDependentColumnAtIndex"
                        "(i)[j] != j"};
        }
    }

    const auto& tab_metadata_ref = table.getTableMetaData();
    if(tab_metadata_ref.getValueForKey("DataRate").getValue<int>() 
       != 600)
        throw Exception{"Test failed: tab_metadata_ref.getValueForKey"
                "(\"DataRate\").getValue<int>() != 600"};
    if(tab_metadata_ref.getValueForKey("Filename").getValue<std::string>()
       != std::string{"/path/to/file"})
        throw Exception{"Test failed: tab_metadata_ref.getValueForKey"
                "(\"Filename\").getValue<std::string>() != std::string"
                "{\"/path/to/file\"}"};

    std::cout << "Test numComponentsPerElement()." << std::endl;
    ASSERT((static_cast<AbstractDataTable&&>
            (DataTable_<double, double    >{})).
            numComponentsPerElement() == 1);
    ASSERT((static_cast<AbstractDataTable&&>
            (DataTable_<double, Vec3      >{})).
            numComponentsPerElement() == 3);
    ASSERT((static_cast<AbstractDataTable&&>
            (DataTable_<double, UnitVec3  >{})).
            numComponentsPerElement() == 3);
    ASSERT((static_cast<AbstractDataTable&&>
            (DataTable_<double, Quaternion>{})).
            numComponentsPerElement() == 4);
    ASSERT((static_cast<AbstractDataTable&&>
            (DataTable_<double, SpatialVec>{})).
            numComponentsPerElement() == 6);

    {
        std::cout << "Test DataTable flattenning constructor for Vec3."
                  << std::endl;
        DataTable_<double, Vec3> tableVec3{};
        tableVec3.setColumnLabels({"col0", "col1", "col2"});
        tableVec3.appendRow(0.1, {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}});
        tableVec3.appendRow(0.2, {{3, 3, 3}, {1, 1, 1}, {2, 2, 2}});
        tableVec3.appendRow(0.3, {{2, 2, 2}, {3, 3, 3}, {1, 1, 1}});

        DataTable_<double, double> tableDouble{tableVec3};
        std::vector<std::string> expLabels{"col0_1", "col0_2", "col0_3",
                                           "col1_1", "col1_2", "col1_3",
                                           "col2_1", "col2_2", "col2_3"};
        ASSERT(tableDouble.getColumnLabels()   == expLabels);
        ASSERT(tableDouble.getNumRows()        == 3);
        ASSERT(tableDouble.getNumColumns()     == 9);
        {
            const auto& row0 = tableDouble.getRowAtIndex(0);
            const auto& row1 = tableDouble.getRowAtIndex(1);
            const auto& row2 = tableDouble.getRowAtIndex(2);
            ASSERT(row0[0] == 1);
            ASSERT(row1[0] == 3);
            ASSERT(row2[0] == 2);
            ASSERT(row0[8] == 3);
            ASSERT(row1[8] == 2);
            ASSERT(row2[8] == 1);
        }

        std::cout << "Test DataTable flatten() for Vec3." << std::endl;
        auto tableFlat = tableVec3.flatten({"_x", "_y", "_z"});
        expLabels = {"col0_x", "col0_y", "col0_z",
                     "col1_x", "col1_y", "col1_z",
                     "col2_x", "col2_y", "col2_z"};
        ASSERT(tableFlat.getColumnLabels()   == expLabels);
        ASSERT(tableFlat.getNumRows()        == 3);
        ASSERT(tableFlat.getNumColumns()     == 9);
        {
            const auto& row0 = tableFlat.getRowAtIndex(0);
            const auto& row1 = tableFlat.getRowAtIndex(1);
            const auto& row2 = tableFlat.getRowAtIndex(2);
            ASSERT(row0[0] == 1);
            ASSERT(row1[0] == 3);
            ASSERT(row2[0] == 2);
            ASSERT(row0[8] == 3);
            ASSERT(row1[8] == 2);
            ASSERT(row2[8] == 1);
        }

        std::cout << "Test DataTable flattenning constructor for Quaternion."
                  << std::endl;
        DataTable_<double, Quaternion> tableQuat{}; 
        tableQuat.setColumnLabels({"col0", "col1", "col2"});
        tableQuat.appendRow(0.1, {{1, 1, 1, 1}, {2, 2, 2, 2}, {3, 3, 3, 3}});
        tableQuat.appendRow(0.2, {{3, 3, 3, 3}, {1, 1, 1, 1}, {2, 2, 2, 2}});
        tableQuat.appendRow(0.3, {{2, 2, 2, 2}, {3, 3, 3, 3}, {1, 1, 1, 1}});

        tableDouble = tableQuat;
        ASSERT(tableDouble.getColumnLabels().size() == 12);
        ASSERT(tableDouble.getNumRows()             == 3);
        ASSERT(tableDouble.getNumColumns()          == 12);

        std::cout << "Test DataTable flattenning constructor for UnitVec3."
                  << std::endl;
        DataTable_<double, Vec3> tableUnitVec3{};
        tableUnitVec3.setColumnLabels({"col0", "col1", "col2"});
        tableUnitVec3.appendRow(0.1, {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}});
        tableUnitVec3.appendRow(0.2, {{3, 3, 3}, {1, 1, 1}, {2, 2, 2}});
        tableUnitVec3.appendRow(0.3, {{2, 2, 2}, {3, 3, 3}, {1, 1, 1}});

        tableDouble = tableUnitVec3;
        ASSERT(tableDouble.getColumnLabels().size() == 9);
        ASSERT(tableDouble.getNumRows()             == 3);
        ASSERT(tableDouble.getNumColumns()          == 9);

        std::cout << "Test DataTable flattenning constructor for SpatialVec."
                  << std::endl;
        DataTable_<double, SpatialVec> tableSpatialVec{};
        tableSpatialVec.setColumnLabels({"col0", "col1", "col2"});
        tableSpatialVec.appendRow(0.1, {{{1, 1, 1}, {1, 1, 1}},
                                        {{2, 2, 2}, {2, 2, 2}},
                                        {{3, 3, 3}, {3, 3, 3}}});
        tableSpatialVec.appendRow(0.2, {{{3, 3, 3}, {3, 3, 3}},
                                        {{1, 1, 1}, {1, 1, 1}},
                                        {{2, 2, 2}, {2, 2, 2}}});
        tableSpatialVec.appendRow(0.3, {{{2, 2, 2}, {2, 2, 2}},
                                        {{3, 3, 3}, {3, 3, 3}},
                                        {{1, 1, 1}, {1, 1, 1}}});

        tableDouble = tableSpatialVec;
        ASSERT(tableDouble.getColumnLabels().size() == 18);
        ASSERT(tableDouble.getNumRows()             == 3);
        ASSERT(tableDouble.getNumColumns()          == 18);
    }
    {
        std::cout << "Test TimeSeriesTable flattenning constructor for Vec3"
                  << std::endl;
        TimeSeriesTable_<Vec3> tableVec3{};
        tableVec3.setColumnLabels({"col0", "col1", "col2"});
        tableVec3.appendRow(0.1, {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}});
        tableVec3.appendRow(0.2, {{3, 3, 3}, {1, 1, 1}, {2, 2, 2}});
        tableVec3.appendRow(0.3, {{2, 2, 2}, {3, 3, 3}, {1, 1, 1}});
        
        TimeSeriesTable_<double> tableDouble{tableVec3};
        std::vector<std::string> expLabels{"col0_1", "col0_2", "col0_3",
                                           "col1_1", "col1_2", "col1_3",
                                           "col2_1", "col2_2", "col2_3"};
        ASSERT(tableDouble.getColumnLabels() == expLabels);
        ASSERT(tableDouble.getNumRows()      == 3);
        ASSERT(tableDouble.getNumColumns()   == 9);
        {
            const auto& row0 = tableDouble.getRowAtIndex(0);
            const auto& row1 = tableDouble.getRowAtIndex(1);
            const auto& row2 = tableDouble.getRowAtIndex(2);
            ASSERT(row0[0] == 1);
            ASSERT(row1[0] == 3);
            ASSERT(row2[0] == 2);
            ASSERT(row0[8] == 3);
            ASSERT(row1[8] == 2);
            ASSERT(row2[8] == 1);
        }

        std::cout << "Test TimeSeriesTable flatten() for Vec3." << std::endl;
        auto tableFlat = tableVec3.flatten({"_x", "_y", "_z"});
        expLabels = {"col0_x", "col0_y", "col0_z",
                     "col1_x", "col1_y", "col1_z",
                     "col2_x", "col2_y", "col2_z"};
        ASSERT(tableFlat.getColumnLabels()   == expLabels);
        ASSERT(tableFlat.getNumRows()        == 3);
        ASSERT(tableFlat.getNumColumns()     == 9);
        {
            const auto& row0 = tableFlat.getRowAtIndex(0);
            const auto& row1 = tableFlat.getRowAtIndex(1);
            const auto& row2 = tableFlat.getRowAtIndex(2);
            ASSERT(row0[0] == 1);
            ASSERT(row1[0] == 3);
            ASSERT(row2[0] == 2);
            ASSERT(row0[8] == 3);
            ASSERT(row1[8] == 2);
            ASSERT(row2[8] == 1);
        }

        std::cout << "Test TimeSeriesTable flattenning constructor for "
                     "Quaternion" << std::endl;
        TimeSeriesTable_<Quaternion> tableQuat{}; 
        tableQuat.setColumnLabels({"col0", "col1", "col2"});
        tableQuat.appendRow(0.1, {{1, 1, 1, 1}, {2, 2, 2, 2}, {3, 3, 3, 3}});
        tableQuat.appendRow(0.2, {{3, 3, 3, 3}, {1, 1, 1, 1}, {2, 2, 2, 2}});
        tableQuat.appendRow(0.3, {{2, 2, 2, 2}, {3, 3, 3, 3}, {1, 1, 1, 1}});

        tableDouble = tableQuat;
        ASSERT(tableDouble.getColumnLabels().size() == 12);
        ASSERT(tableDouble.getNumRows()             == 3);
        ASSERT(tableDouble.getNumColumns()          == 12);

        std::cout << "Test TimeSeriesTable flattenning constructor for UnitVec3"
                  << std::endl;
        TimeSeriesTable_<Vec3> tableUnitVec3{};
        tableUnitVec3.setColumnLabels({"col0", "col1", "col2"});
        tableUnitVec3.appendRow(0.1, {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}});
        tableUnitVec3.appendRow(0.2, {{3, 3, 3}, {1, 1, 1}, {2, 2, 2}});
        tableUnitVec3.appendRow(0.3, {{2, 2, 2}, {3, 3, 3}, {1, 1, 1}});

        tableDouble = tableUnitVec3;
        ASSERT(tableDouble.getColumnLabels().size() == 9);
        ASSERT(tableDouble.getNumRows()             == 3);
        ASSERT(tableDouble.getNumColumns()          == 9);

        std::cout << "Test TimeSeriesTable flattenning constructor for "
                     "SpatialVec" << std::endl;
        TimeSeriesTable_<SpatialVec> tableSpatialVec{};
        tableSpatialVec.setColumnLabels({"col0", "col1", "col2"});
        tableSpatialVec.appendRow(0.1, {{{1, 1, 1}, {1, 1, 1}},
                                        {{2, 2, 2}, {2, 2, 2}},
                                        {{3, 3, 3}, {3, 3, 3}}});
        tableSpatialVec.appendRow(0.2, {{{3, 3, 3}, {3, 3, 3}},
                                        {{1, 1, 1}, {1, 1, 1}},
                                        {{2, 2, 2}, {2, 2, 2}}});
        tableSpatialVec.appendRow(0.3, {{{2, 2, 2}, {2, 2, 2}},
                                        {{3, 3, 3}, {3, 3, 3}},
                                        {{1, 1, 1}, {1, 1, 1}}});

        tableDouble = tableSpatialVec;
        ASSERT(tableDouble.getColumnLabels().size() == 18);
        ASSERT(tableDouble.getNumRows()             == 3);
        ASSERT(tableDouble.getNumColumns()          == 18);
    }

    return 0;
}
