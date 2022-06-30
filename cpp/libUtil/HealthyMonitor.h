	struct HealthyMonitor {
		HealthyMonitor() {
			reset();
		}
		void update(const float dt) {
			cnt++;
			t_min = APP_MIN(dt, t_min);
			t_max = APP_MAX(dt, t_max);
			t_mean += ((float)dt - t_mean) / ((float)cnt);
		}

		void reset() {
			if (cnt > 0) {
				reset_cnt++;
				reset_min = APP_MIN(t_max, reset_min);
				reset_max = APP_MAX(t_max, reset_max);
				reset_mean += (t_max - reset_mean) / (float)reset_cnt;
			}

			t_min  = std::numeric_limits<float>::max();
			t_max  = std::numeric_limits<float>::min();
			t_mean = 0;
			cnt = 0;

		}

		std::string to_string( const std::string &msg="", const std::string &tag="") {
			char buf[1024];
			snprintf(buf, 1024, "(%s_min=%.1f, max=%.1f, mean=%.1f), cnt=%u, reset: (min=%.1f,max=%.1f,mean=%.1f), resetCnt=%u",
				tag.c_str(), t_min, t_max, t_mean, cnt, reset_min, reset_max, reset_mean, reset_cnt);

			return msg + std::string(buf);
		}

		//inside reset
		float  t_min{ std::numeric_limits<float>::max() };
		float  t_max{ std::numeric_limits<float>::min() };
		float  t_mean{ 0 };
		uint32_t cnt{ 0 };

		//acrossing reset
		uint32_t reset_cnt{ 0 };
		float  reset_min{ std::numeric_limits<float>::max() };
		float  reset_max{ std::numeric_limits<float>::min() };
		float  reset_mean{ 0 };
	};
