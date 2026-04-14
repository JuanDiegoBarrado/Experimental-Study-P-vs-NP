#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "boolean_functions/boolean_functions.h"
#include "metrics/metrics.h"

ull num_ones(const truthtable& sequence) {
    int n_ones = 0;
    for (auto bit : sequence)
        if (bit)
            n_ones++;
    return n_ones;
}

ull vc_values(const truthtable& sequence) {
    int n = sequence.size();
    int k = n;

    vector<vector<ull>> dp(n + 1, vector<ull>(k + 1, 0));
    vector<vector<vector<ull>>> terminan(n + 1, vector<vector<ull>>(k + 1, vector<ull>(2, 0)));

    // dp[i][i] = 0 (already initialized)
    // dp[i][0] = 1
    for (int i = 0; i <= n; i++)
        dp[i][0] = 1;

    
    for (int i = 1; i <= n; i++) {
        int char_val = bool(sequence[i - 1]) ? 1 : 0;
        int neg_char_val = 1 - char_val;
        for (int j = 1; j < min(i + 1, k + 1); j++) {
            dp[i][j] = dp[i-1][j] + (dp[i-1][j-1] - terminan[i-1][j][char_val]);
            terminan[i][j][char_val] = terminan[i-1][j][char_val] + (dp[i-1][j-1] - terminan[i-1][j][char_val]);
            terminan[i][j][neg_char_val]  = dp[i][j] - terminan[i][j][char_val];
        }
    }

    ull total = 0;
    for (int j = 0; j <= k; j++)
        total += dp[n][j];
    return total;
}