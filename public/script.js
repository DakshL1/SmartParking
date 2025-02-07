function updateCarPositions() {
    fetch('http://192.168.146.111/')  
        .then(response => response.json())
        .then(data => {
            console.log('Distance data:', data);

            const hideThreshold = 150; // Distance above which the car is hidden 
            const maxDistance = 100;  // Max distance that the car will move within the slot 

            // Function to update car position smoothly
            function moveCar(car, distance, slotHeight) {
                if (car) {
                    if (distance > hideThreshold) {
                        car.style.display = 'none'; // Hide car if no object in front
                    } else {
                        car.style.display = 'block'; // Show car

                        // Calculate the position based on the distance (move car vertically)
                        let movement = Math.min(distance, maxDistance); // Ensure car doesn't move beyond the bottom of the slot
                        let position = (movement / maxDistance) * slotHeight; // Scale to slot height (percentage)

                        

                        // Move the car from the top to the bottom based on distance
                        car.style.transform = `translateY(${position}px)`; // Start from top and move down
                    }
                }
            }

            // Move car for Slot 1
            let car1 = document.getElementById('car1');
            let distance1 = data.distance1;
            let slot1Height = document.querySelector('.slot-1').offsetHeight; // Get the height of the slot
            moveCar(car1, distance1, slot1Height);

            // Move car for Slot 2
            let car2 = document.getElementById('car2');
            let distance2 = data.distance2;
            let slot2Height = document.querySelector('.slot-2').offsetHeight; // Get the height of the slot
            moveCar(car2, distance2, slot2Height);

            // Move car for Slot 3
            let car3 = document.getElementById('car3');
            let distance3 = data.distance3;
            let slot3Height = document.querySelector('.slot-3').offsetHeight; // Get the height of the slot
            moveCar(car3, distance3, slot3Height);

            // Move car for Slot 4
            let car4 = document.getElementById('car4');
            let distance4 = data.distance4;
            let slot4Height = document.querySelector('.slot-4').offsetHeight; // Get the height of the slot
            moveCar(car4, distance4, slot4Height);

            // Move car for Slot 5
            let car5 = document.getElementById('car5');
            let distance5 = data.distance5;
            let slot5Height = document.querySelector('.slot-5').offsetHeight; // Get the height of the slot
            moveCar(car5, distance5, slot5Height);

        })
        .catch(error => console.error('Error fetching data:', error));
}

setInterval(updateCarPositions, 100);
