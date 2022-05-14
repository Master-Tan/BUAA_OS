void FCFS (
    int number_of_jobs,
    const int job_submitted_time [],
    const int job_required_time [],
    int job_sched_start []
) {
	int i;
	int time;
	for (i = 0, time = 0; i < number_of_jobs; i++) {
		if (time < job_submitted_time[i]) {
			time = job_submitted_time[i];
		}
		job_sched_start[i] = time;
		time += job_required_time[i];
	}
}

void SJF (
    int number_of_jobs,
    const int job_submitted_time [],
    const int job_required_time [],
    int job_sched_start []
) {
	int i, j;
	int time;
	int done[number_of_jobs];
	int choose_job;
	int choose_time;
	for (i = 0, time = 0; i < number_of_jobs; i++) {
		done[i] = 0;
	}
	for (i = 0, time = 0; i < number_of_jobs; i++) {
		choose_job = -1;
		choose_time = 99999999;
		for (j = 0; j < number_of_jobs; j++) {
			if (done[j] == 0 && choose_time > job_required_time[j] &&
					time >= job_submitted_time[j]) {
				choose_job = j;
				choose_time = job_required_time[j];
			}
		}
		int first = 9999999;
		if (choose_job == -1) {
			for (j = 0; j < number_of_jobs; j++) {
				if (done[j] == 0 && first > job_submitted_time[j]) {
					first = job_submitted_time[j];
					choose_job = j;
					choose_time = job_required_time[j];
				} else if (done[j] == 0 && first == job_submitted_time[j]) {
					if (choose_time > job_required_time[j]) {
						choose_job = j;
						choose_time = job_required_time[j];
					}
				}
			}
		}
		done[choose_job] = 1;
		if (time < job_submitted_time[choose_job]) {
			time = job_submitted_time[choose_job];
		}
		job_sched_start[choose_job] = time;
		time += job_required_time[choose_job];
	}
}
